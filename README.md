# mpi-vscode-template

Contains my setup for MPI schoolwork in a VSCode Workspace. Ideally used in 
conjunction with the WSL Remote on Windows. 

This lets me do two things: 
1. Use VSCode tasks to run my code with an arbitrary number of processes.
2. Create pretty output without too much (read: any) work.

## Scripts

* `run.sh` - Finds all source files in the project, compiles, and runs them. 
Used by the VSCode task. Takes the number of processes as an argument.
* `package.sh` - Collects all source files in the project, collapses the 
structure (moves `src/...` includes to just `...`), and zips it with the
`packageRun.sh` script.
* `packageRun.sh` turns into the `run.sh` script in the packaged project.
Takes the number of processes as an argument.

## Examples

### Random Numbers (Default Source)

**C++**
```cpp
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

```

**Output (8 processes):**
```
Random Table
┌───────────────────────────────────────────────┐
│              random_process_data              │
├─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┤
│  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│ 583 │ 583 │  90 │ 346 │ 701 │ 475 │ 541 │ 277 │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

**Output (16 processes):**
```
Random Table
┌───────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                              random_process_data                                              │
├──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┤
│   0  │   1  │   2  │   3  │   4  │   5  │   6  │   7  │   8  │   9  │  10  │  11  │  12  │  13  │  14  │  15  │
├──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┼──────┤
│ 1383 │ 1383 │  890 │ 1146 │ 1501 │  475 │ 1341 │ 1077 │  696 │  515 │  495 │ 1023 │ 1360 │  490 │  283 │ 1493 │
└──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┘
```

### Bitonic Sorting

**C++**
```cpp
// This file is provided with this setup.
#include "src/mpiu.hpp"
// This file is not.
#include "sort.hpp"

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank);
    int data = (rand() % (size*10));

    debugh("Pre-Sorting:");
    debugt(data);
    MPI_Barrier(MCW);

    data = sort(rank, size, data);
    MPI_Barrier(MCW);

    debugh("Post-Sorting:");
    debugt(data);
    MPI_Barrier(MCW);

    MPI_Finalize();
}
```

**Output (8 processes):**

```
Pre-Sorting:
┌───────────────────────────────────────┐
│                  data                 │
├────┬────┬────┬────┬────┬────┬────┬────┤
│  0 │  1 │  2 │  3 │  4 │  5 │  6 │  7 │
├────┼────┼────┼────┼────┼────┼────┼────┤
│ 23 │ 23 │ 10 │ 26 │ 61 │ 75 │ 61 │ 37 │
└────┴────┴────┴────┴────┴────┴────┴────┘
Post-Sorting:
┌───────────────────────────────────────┐
│                  data                 │
├────┬────┬────┬────┬────┬────┬────┬────┤
│  0 │  1 │  2 │  3 │  4 │  5 │  6 │  7 │
├────┼────┼────┼────┼────┼────┼────┼────┤
│ 10 │ 23 │ 23 │ 26 │ 37 │ 61 │ 61 │ 75 │
└────┴────┴────┴────┴────┴────┴────┴────┘
```

**Output (16 processes):**
```
Pre-Sorting:
┌───────────────────────────────────────────────────────────────────────────────────────────────┐
│                                              data                                             │
├─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┤
│  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │  8  │  9  │  10 │  11 │  12 │  13 │  14 │  15 │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│ 103 │ 103 │  90 │  26 │  61 │ 155 │  61 │ 117 │  56 │  35 │  15 │  63 │  80 │  10 │ 123 │  53 │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
Post-Sorting:
┌───────────────────────────────────────────────────────────────────────────────────────────────┐
│                                              data                                             │
├─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┤
│  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │  8  │  9  │  10 │  11 │  12 │  13 │  14 │  15 │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│  10 │  15 │  26 │  35 │  53 │  56 │  61 │  61 │  63 │  80 │  90 │ 103 │ 103 │ 117 │ 123 │ 155 │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```