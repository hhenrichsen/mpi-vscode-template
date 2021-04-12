#include "src/mpiu.hpp"
#include "src/mpitype.hpp"
#include "src/mpiwrapper.hpp"

bool run(MPIWrapper mpi) {
    srand(mpi.getRank() * 100);
    long send = rand() % (mpi.getSize() * 100);
    mpi.table<long>(send, "First");
    mpi.sendRing<long>(send);
    long recv = mpi.receive<long>();
    mpi.table<long>(recv, "Second (Ring)");
    mpi.sendCube<long>(recv, 0, 1);
    recv = mpi.receiveTagged<long>(1);
    mpi.table<long>(recv, "Third (0th Degree Cube)");
    mpi.sendCube<long>(recv, 1);
    recv = mpi.receive<long>();
    mpi.table<long>(recv, "Fourth (1st Degree Cube)");
    return true;
}

int main(int argc, char** argv) {
    MPIWrapper mpi(argc, argv);
    mpi.setWorkFunction(run);
    mpi.work();
}
