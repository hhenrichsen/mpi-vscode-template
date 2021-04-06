#include "src/mpiu.hpp"
#include "src/mpitype.hpp"
#include "src/mpiwrapper.hpp"

bool run(MPIWrapper wrapper) {
    srand(wrapper.getRank() * 100);
    long send = rand() % (wrapper.getSize() * 100);
    wrapper.table<long>(send, "First");
    wrapper.sendRing<long>(send);
    long recv = wrapper.receive<long>();
    wrapper.table<long>(recv, "Second (Ring)");
    wrapper.sendCube<long>(recv, 0, 1);
    recv = wrapper.receiveTagged<long>(1);
    wrapper.table<long>(recv, "Third (0th Degree Cube)");
    wrapper.sendCube<long>(recv, 1);
    recv = wrapper.receive<long>();
    wrapper.table<long>(recv, "Fourth (1st Degree Cube)");
    return true;
}

int main(int argc, char** argv) {
    MPIWrapper wrapper(argc, argv);
    wrapper.setWorkFunction(run);
    wrapper.work();
}
