#!/bin/bash
mpic++ -std=c++11 *.hpp *.cpp && mpirun -np $1 -oversubscribe ./a.out
rm a.out