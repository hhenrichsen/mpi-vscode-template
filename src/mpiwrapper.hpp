#ifndef MPI_WRAPPER_HPP
#define MPI_WRAPPER_HPP
#include <mpi.h>
#include <functional>
#include <queue>
#include "mpitype.hpp"
#include "mpiu.hpp"

#define MCW MPI_COMM_WORLD

/**
 * MPIWrapper, an MPI Utility class by Hunter Henrichsen and Sally Devitry.
 * 
 * Provides a wrapper around common MPI functions, intitialization, and
 * finalization. Provides sane defaults to functions so that only the bare
 * minimum information is required.
 */
class MPIWrapper {
private:
    MPI_Comm world;
    int size;
    int rank;
    int scopes = 1;
    std::function<bool (MPIWrapper*)> work_fn;
    MPI_Status* lastStatus;

    void updateStatus(MPI_Status* other); 
public:
    // Basic setup
    /**
     * Constructor. Sets up the MPI process, fills rank and size, and 
     * establishes status monitorring.
     * 
     * @param argc The number of arguments in argv.
     * @param argv The vector of command-line arguments.
     */
    MPIWrapper(int argc, char** argv);
    MPIWrapper(const MPIWrapper& other);

    /**
     * Deconstructor. Finalizes MPI after ensuring that all requests that it
     * was waiting for are fulfilled.
     */
    ~MPIWrapper();

    // Ranks

    /**
     * @returns The nuumber of processes in this MPI environment.
     * 
     * @order O(1).
     */
    int getSize();

    /**
     * @returns The rank of this process.
     * 
     * @order O(1).
     */
    int getRank();

    /**
     * @returns The next rank in this MPI environment, looping around back to
     * 0 once it reaches the size.
     * 
     * @order O(1).
     */
    int getNextRank();

    /**
     * @returns The previous rank in this MPI environment, looping around back
     * to size-1 once it reaches the beginning.
     * 
     * @order O(1).
     */
    int getPrevRank();

    /**
     * @returns The given rank but wrapped to fall within the proper range of
     * valid ranks within this MPI Environment.
     * 
     * @order O(1).
     */
    int getOffset(int rank);

    /**
     * @returns A random rank within the proper range of valid ranks within 
     * this MPI Environment.
     * 
     * @order O(1).
     */
    int getRandomRank();

    /**
     * @returns The cube partner of this rank in the given dimension.
     * 
     * @order O(1).
     */
    int getCubeRank(const int dimension);

    // Barrier

    /**
     * Waits for all processes in this environment to reach this point in
     * code, then continues.
     */
    void barrier();
    
    // Check for data.

    /**
     * @returns If this process has waiting communication.
     */
    bool hasData();

    /**
     * @param source The source to check for communication from.
     *
     * @returns If this process has waiting communication to be read from
     * source.
     */
    bool hasData(int source);

    /**
     * @param source The source to check for communication from.
     * @param tag The tag to check for communication from.
     *
     * @returns If this process has waiting communication to be read from
     * source that's tagged with tag.
     */
    bool hasData(int source, int tag);

    /**
     * @param source The source to check for communication from.
     * @param tag The tag to check for communication from.
     * @param status The status to be updated on communication.
     *
     * @returns If this process has waiting communication to be read from
     * source that's tagged with tag.
     */
    bool hasData(int source, int tag, MPI_Status* status);

    /**
     * Prints data from each process in an ordered tabular format. Uses a
     * reduce behind the scenes.
     * 
     * @param data The data to insert into the table.
     * @param name The header to place at the top of the table.
     * @param T The MPI-supported type to put in the table.
     */
    template <typename T>
    void table(T data, std::string name) {
        debug_table<T>(this->rank, this->size, name, data);
    }

    template<typename T>
    void send(const T& data, const int& destination, const int& tag=0) {
        T tmp = data;
        MPI_Send(&tmp, 1, mpi_type<T>::get(), destination, tag, this->world);
    }

    template<typename T>
    void sendRing(const T& data, const int& tag=0) {
        send<T>(data, getNextRank(), tag);
    }

    template<typename T>
    void sendCube(const T& data, const int& dimension, const int& tag=0) {
        send<T>(data, getCubeRank(dimension), tag);
    }

    template<typename T>
    void sendMultiple(const T* data, const int& count, const int& destination, const int& tag=0) {
        MPI_Send(&data, count, mpi_type<T>::get(), destination, tag, this->world);
    }

    template<typename T>
    void sendMultipleRing(const T* data, const int& count, const int& tag=0) {
        sendMultiple<T>(data, count, getNextRank(), tag);
    }

    template<typename T>
    void sendMultipleCube(const T* data, const int& count, const int& dimension, const int& tag=0) {
        sendMultiple<T>(&data, count, mpi_type<T>::get(), getCubeRank(dimension), tag, this->world);
    }

    template<typename T>
    T receive(const int& source, const int& tag, MPI_Status*& status) {
        T tmp;
        MPI_Recv(&tmp, 1, mpi_type<T>::get(), source, tag, this->world, this->lastStatus);
        updateStatus(status);
        return tmp;
    }

    template<typename T>
    T receive(const int& source=MPI_ANY_SOURCE, const int& tag=MPI_ANY_TAG) {
        return receive<T>(source, tag, lastStatus);
    }

    template<typename T>
    T receiveTagged(const int& tag, MPI_Status*& status) {
        return receive<T>((const int&) MPI_ANY_SOURCE, tag, status);
    }

    template<typename T>
    T receiveTagged(const int& tag) {
        return receive<T>((const int&) MPI_ANY_SOURCE, tag, lastStatus);
    }

    template<typename T>
    T* receiveMultiple(const int& count, const int& source, const int& tag, MPI_Status*& status) {
        T* tmp = new T[count];
        MPI_Recv(&tmp, count, mpi_type<T>::get(), source, tag, this->world, this->lastStatus);
        updateStatus(status);
        return tmp;
    }

    template<typename T>
    T* receiveMultiple(const int& count, const int& source=MPI_ANY_SOURCE, const int& tag=MPI_ANY_TAG) {
        return receiveMultiple<T>(count, source, tag, lastStatus);
    }

    template<typename T>
    T* receiveMultipleTagged(const int& count, const int& tag, MPI_Status*& status) {
        return receiveMultiple<T>(count, (const int&) MPI_ANY_SOURCE, tag, status);
    }

    template<typename T>
    T* receiveMultipleTagged(const int& count, const int& tag) {
        return receiveMultiple<T>(count, (const int&) MPI_ANY_SOURCE, tag, lastStatus);
    }

    // Status-related utilities
    MPI_Status getLastStatus();
    int getLastSource();
    int getLastTag();

    // Functional work setup
    void setWorkFunction(std::function<bool (MPIWrapper*)> work_fn);
    void work();

    // Printing
    void print(std::string message);
};

#endif // MPI_WRAPPER_HPP