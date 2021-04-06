#!/bin/bash
HEADERS=$(find . -name "*.hpp" -print)
IMPL=$(find . -name "*.cpp" -print)
mpic++ -std=c++11 -g $HEADERS $IMPL && mpirun -np $1 -oversubscribe valgrind ./a.out
rm a.out