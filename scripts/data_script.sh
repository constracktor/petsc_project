#!/bin/bash
# since Bash v4
for i in {5000..100000..5000}
do
     time mpirun -n 6 ../src/petsc_cholesky -n_train $i -n_test 5000
done
