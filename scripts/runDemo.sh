#!/bin/bash
HEADERS=$(find . -name "*.hpp" -print)
IMPL=$(find ./src -name "*.cpp" -print)
mpic++ -std=c++11 demos/$1.cpp $HEADERS $IMPL && mpirun -np $2 -oversubscribe ./a.out
rm a.out