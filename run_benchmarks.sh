#!/bin/bash
# Set variables
export OMPI_MCA_btl_vader_single_copy_mechanism=none
export PETSC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )/dependencies/src/petsc"
export RUN_COMMAND="${PETSC_DIR}/lib/petsc/bin/petscmpiexec"
# decide for Release or Debug config (dependant on PETSc installation)
if [[ "$1" == "Release" ]]
then
    DEBUGGING=0
    export PETSC_ARCH=arch-linux-cxx-opt
else
    DEBUGGING=1
    export PETSC_ARCH=arch-linux-cxx-debug
fi
# Compile Code
cd src && make petsc_cholesky DEBUG=${DEBUGGING}
# Run both scripts
cd ../benchmark_scripts
OUTPUT_FILE_CORES="cores_petsc.txt"
OUTPUT_FILE_DATA="data_petsc.txt"
rm $OUTPUT_FILE_CORES
rm $OUTPUT_FILE_DATA
LOOP=5
# Run cores_script
START=1
END=128
STEP=2
N_TRAIN=20000
N_TEST=5000
N_REG=100
#./cores_script.sh $START $END $STEP $N_TRAIN $N_TEST $N_REG $LOOP $OUTPUT_FILE_CORES| tee -a $OUTPUT_FILE_CORES
# Run data_script up to 1 000
START=100
END=900
STEP=100
N_CORES=1
N_TEST=1000
N_REG=100
./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA
# Run data_script up to 10 000
START=1000
END=10000
STEP=1000
N_CORES=1
N_TEST=1000
N_REG=100
./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA
# Run data_script from 10 000 to 100 000
START=10000
END=100000
STEP=10000
N_CORES=128
N_TEST=5000
N_REG=100
#./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA

# 128;50000;5000;100;207.993578;13.849659;194.126765;0.017153;0.004673; -> 4 min
# 128;100000;5000;100;1409.708050;54.244647;1355.396655;0.066746;0.004567; -> 23 min
