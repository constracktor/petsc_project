#!/bin/bash
touch data_result.txt && echo 'Cores;Total_time;Assemble_time;Solve_time;Predict_time;Error;N_train;N_test;N_regressor;' >> data_result.txt
cd ../src
START=$1
END=$2
STEP=$3
N_CORES=$4
N_TEST=$5
N_REG=$6
for (( i=$START; i<= $END; i=i+$STEP ))
do
     ${RUN_COMMAND} -n $N_CORES ../src/petsc_cholesky -n_train $i -n_test $N_TEST -n_regressors $N_REG
done
