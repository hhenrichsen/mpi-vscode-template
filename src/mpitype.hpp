#ifndef MPI_TYPE_HPP
#define MPI_TYPE_HPP
#include "mpi.h"

template<typename T> struct mpi_type
{
};

#define MAKE_MPI_TYPE(x, y) template<> struct mpi_type<x> { static const MPI_Datatype get() { return y; } }

MAKE_MPI_TYPE(char, MPI_CHAR);
MAKE_MPI_TYPE(short, MPI_SHORT);
MAKE_MPI_TYPE(int, MPI_INT);
MAKE_MPI_TYPE(long, MPI_LONG);
MAKE_MPI_TYPE(long long, MPI_LONG_LONG);
MAKE_MPI_TYPE(unsigned char, MPI_UNSIGNED_CHAR);
MAKE_MPI_TYPE(unsigned short, MPI_UNSIGNED_SHORT);
MAKE_MPI_TYPE(unsigned int, MPI_UNSIGNED);
MAKE_MPI_TYPE(unsigned long, MPI_UNSIGNED_LONG);
MAKE_MPI_TYPE(unsigned long long, MPI_UNSIGNED_LONG_LONG);
MAKE_MPI_TYPE(float, MPI_FLOAT);
MAKE_MPI_TYPE(double, MPI_DOUBLE);
MAKE_MPI_TYPE(long double, MPI_LONG_DOUBLE);

#endif // MPI_TYPE_HPP