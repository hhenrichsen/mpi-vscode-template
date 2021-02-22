#include "src/mpiu.hpp"

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank);
    // Try changing this variable's name.
    int random_process_data = (rand() % (size*100));

    debugh("Random Table");
    debugt(random_process_data);

    MPI_Finalize();
}
