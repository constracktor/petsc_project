#!/bin/bash
START=$1
END=$2
STEP=$3
N_CORES=$4
N_TEST=$5
N_REG=$6
N_LOOP=$7
OUTPUT_FILE=$8

touch $OUTPUT_FILE && echo "Cores;N_train;N_test;N_regressor;Total_time;Assemble_time;Solve_time;Predict_time;Error;${N_LOOP}" >> $OUTPUT_FILE

cd ../src
for (( i=$START; i<= $END; i=i+$STEP ))
do
  for (( l=0; l<$N_LOOP; l=l+1 ))
  do
     ${RUN_COMMAND} -n $N_CORES ../src/petsc_cholesky -n_train $i -n_test $N_TEST -n_regressors $N_REG
  done
done
