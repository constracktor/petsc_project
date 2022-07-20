#!/bin/bash
# since Bash v4
cd ../src
for i in {1..6..1}
do
     mpirun -n $i ../src/petsc_cholesky -n_train 1000 -n_test 1000
done
