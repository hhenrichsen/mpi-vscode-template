#include "mpiwrapper.hpp"
#include "mpiu.hpp"
#include "mpitype.hpp"
#include <cmath>

MPIWrapper::MPIWrapper(const MPIWrapper& other) : 
    rank(other.rank), size(other.size), lastStatus(other.lastStatus),
    world(other.world) {
    this->scopes++;
}

MPIWrapper::MPIWrapper(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    this->world = MPI_COMM_WORLD;
    int rank_temp;
    int size_temp;
    MPI_Comm_rank(world, &rank_temp);
    MPI_Comm_size(world, &size_temp);
    this->rank = rank_temp;
    this->size = size_temp;
    this->lastStatus = new MPI_Status();
}

MPIWrapper::~MPIWrapper() {
    if (--(this->scopes) == 0) {
        MPI_Finalize();
        delete this->lastStatus;
    }
}

int MPIWrapper::getRank() {
    return this->rank;
}

int MPIWrapper::getCubeRank(const int dimension) {
    return (getRank() ^ (1 << dimension));
}

int MPIWrapper::getNextRank() {
    return (getRank() + 1) % getSize();
}

int MPIWrapper::getPrevRank() {
    if (getRank() != 0) {
        return (getRank() - 1) % getSize();
    }
    return getSize() - 1;
}

int MPIWrapper::getOffset(int rank) {
    int size = getSize();
    int tmp = -rank / size;
    return rank + (size * tmp) + size - 1;
}

int MPIWrapper::getSize() {
    return this->size;
}

bool MPIWrapper::hasData(int source, int flag, MPI_Status* status) {
    int found;
    MPI_Iprobe(source, flag, this->world, &found, lastStatus);
    updateStatus(status);
    return found != 0;
}

bool MPIWrapper::hasData(int source, int flag) {
    return hasData(source, flag, this->lastStatus);
}

bool MPIWrapper::hasData(int source) {
    return hasData(source, -1, this->lastStatus);
}

bool MPIWrapper::hasData() {
    return hasData(MPI_ANY_SOURCE);
}

void MPIWrapper::updateStatus(MPI_Status* other) {
    if (this->lastStatus != other) {
        *other = *(this->lastStatus);
    }
}

MPI_Status MPIWrapper::getLastStatus() {
    return *(this->lastStatus);
}

int MPIWrapper::getLastSource() {
    return this->lastStatus->MPI_SOURCE;
}

int MPIWrapper::getLastTag() {
    return this->lastStatus->MPI_TAG;
}

void MPIWrapper::setWorkFunction(std::function<bool (MPIWrapper)> fn) {
    this->work_fn = fn;
}

void MPIWrapper::work() {
    bool done = this->work_fn(*this);
    while (!done) {
        std::cout << "Iterating again..." << std::endl;
        done = this->work_fn(*this);
    }
}

void MPIWrapper::barrier() {
    MPI_Barrier(this->world);
}

int MPIWrapper::getRandomRank() {
    int destination = (rand() % this->getSize());
    if (destination == this->getRank()) {
        return this->getNextRank();
    }
    return destination;
}

void MPIWrapper::print(std::string message) {
    std::cout << "Process " << getRank() << ": " << message << std::endl;
}