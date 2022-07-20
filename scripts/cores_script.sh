#!/bin/bash
touch cores_result.txt && echo 'Cores;Time;Error;N_train;N_test;N_regressor;' >> cores_result.txt
cd ../src
START=$1
END=$2
STEP=$3
for (( i=$START; i<= $END; i=i+$STEP ))
do
     mpirun -n $i ../src/petsc_cholesky -n_train 1000 -n_test 1000
done
