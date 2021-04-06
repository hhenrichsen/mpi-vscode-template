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
    std::function<bool (MPIWrapper)> work_fn;
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
     * Prints a value from each process in an ordered tabular format. Uses a
     * reduce behind the scenes.
     * 
     * @param value The value to insert into the table.
     * @param name The header to place at the top of the table.
     * @param T The MPI-supported type to put in the table.
     */
    template <typename T>
    void table(T value, std::string name) {
        debug_table<T>(this->rank, this->size, name, value);
    }

    /**
     * Sends a value from one process to another.
     * 
     * @param value The value to send to the specified process.
     * @param destination The rank of the specified process.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void send(const T& value, const int& destination, const int& tag=0) {
        T tmp = value;
        MPI_Send(&tmp, 1, mpi_type<T>::get(), destination, tag, this->world);
    }

    /**
     * Sends a value to the next process using a ring topology.
     * 
     * @param value The value to send to the next process.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void sendRing(const T& value, const int& tag=0) {
        send<T>(value, getNextRank(), tag);
    }

    /**
     * Sends a value from one process to another in a cube topology.
     * 
     * @param value The value to send to the next process.
     * @param dimension The cube dimension to send value along.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void sendCube(const T& value, const int& dimension, const int& tag=0) {
        send<T>(value, getCubeRank(dimension), tag);
    }

    /**
     * Sends values from one process to another.
     * 
     * @param values The values to send to the next process.
     * @param count The number of values being sent.
     * @param destination The rank of the specified process.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void sendMultiple(const T* values, const int& count, const int& destination, const int& tag=0) {
        MPI_Send(&values, count, mpi_type<T>::get(), destination, tag, this->world);
    }

    /**
     * Sends values from one process to another in a ring topology.
     * 
     * @param values The values to send to the next process.
     * @param count The number of values being sent.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void sendMultipleRing(const T* values, const int& count, const int& tag=0) {
        sendMultiple<T>(values, count, getNextRank(), tag);
    }

    /**
     * Sends values from one process to another in a cube topology.
     * 
     * @param values The values to send to the next process.
     * @param count The number of values being sent.
     * @param dimension The cube dimension to send value along.
     * @param tag The tag to send the value with. Defaults to 0.
     * @param T The MPI-supported type to send.
     */
    template<typename T>
    void sendMultipleCube(const T* values, const int& count, const int& dimension, const int& tag=0) {
        sendMultiple<T>(&values, count, mpi_type<T>::get(), getCubeRank(dimension), tag, this->world);
    }

    /**
     * Receives a value from the given source with the given tag and returns
     * it, placing the status in the given status.
     * 
     * @param source The source to receive the value from.
     * @param tag The tag that the received value must match.
     * @param status The status reference to place the status in.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T receive(const int& source, const int& tag, MPI_Status*& status) {
        T tmp;
        MPI_Recv(&tmp, 1, mpi_type<T>::get(), source, tag, this->world, this->lastStatus);
        updateStatus(status);
        return tmp;
    }

    /**
     * Receives a value from the given source with the given tag and returns
     * it.
     * 
     * @param source The source to receive the value from. Defaults to allow any.
     * @param tag The tag that the received value must match. Defaults to allow any.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T receive(const int& source=MPI_ANY_SOURCE, const int& tag=MPI_ANY_TAG) {
        return receive<T>(source, tag, lastStatus);
    }

    /**
     * Receives a value from the given source with the given tag and returns
     * it, placing the status in the given status.
     * 
     * @param tag The tag that the received value must match.
     * @param status The status reference to place the status in.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T receiveTagged(const int& tag, MPI_Status*& status) {
        return receive<T>(MPI_ANY_SOURCE, tag, status);
    }

    /**
     * Receives a value from the given source with the given tag and returns
     * it.
     * 
     * @param tag The tag that the received value must match.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T receiveTagged(const int& tag) {
        return receive<T>(MPI_ANY_SOURCE, tag, lastStatus);
    }

    /**
     * Receives multiple value from the given source with the given tag and
     * returns it, placing the status in the given status.
     * 
     * @param count The number of values to receive.
     * @param source The source to receive the value from.
     * @param tag The tag that the received value must match.
     * @param status The status reference to place the status in.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T* receiveMultiple(const int& count, const int& source, const int& tag, MPI_Status*& status) {
        T* tmp = new T[count];
        MPI_Recv(&tmp, count, mpi_type<T>::get(), source, tag, this->world, this->lastStatus);
        updateStatus(status);
        return tmp;
    }

    /**
     * Receives multiple value from the given source with the given tag and
     * returns it.
     * 
     * @param count The number of values to receive.
     * @param source The source to receive the value from. Defaults to allow any.
     * @param tag The tag that the received value must match. Defaults to allow any.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T* receiveMultiple(const int& count, const int& source=MPI_ANY_SOURCE, const int& tag=MPI_ANY_TAG) {
        return receiveMultiple<T>(count, source, tag, lastStatus);
    }

    /**
     * Receives multiple value from the given source with the given tag and
     * returns it, placing the status in the given status.
     * 
     * @param count The number of values to receive.
     * @param tag The tag that the received value must match.
     * @param status The status reference to place the status in.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T* receiveMultipleTagged(const int& count, const int& tag, MPI_Status*& status) {
        return receiveMultiple<T>(count, MPI_ANY_SOURCE, tag, status);
    }

    /**
     * Receives multiple value from the given source with the given tag and
     * returns it.
     * 
     * @param count The number of values to receive.
     * @param tag The tag that the received value must match. Defaults to allow any.
     * @param T The MPI-supported type to receive.
     * 
     * @return The value that was received.
     */
    template<typename T>
    T* receiveMultipleTagged(const int& count, const int& tag) {
        return receiveMultiple<T>(count, MPI_ANY_SOURCE, tag, lastStatus);
    }

    /**
     * @returns The status from the last receive request.
     */
    MPI_Status getLastStatus();

    /**
     * @returns The source of the last value received.
     */
    int getLastSource();

    /**
     * @returns The tag of the last value received.
     */
    int getLastTag();

    /**
     * Sets the work function. The wrapper will continue executing this
     * function until it returns true.
     * 
     * @param work_fn The work function to set.
     */
    void setWorkFunction(std::function<bool (MPIWrapper)> work_fn);

    /**
     * Runs the work function, if it exists.
     */
    void work();

    /**
     * Sends a message with an attached process indicator.
     * 
     * @param message The message to send.
     */
    void print(std::string message);
};

#endif // MPI_WRAPPER_HPP