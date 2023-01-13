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
cd src
make petsc_blas DEBUG=${DEBUGGING}
make petsc_cholesky DEBUG=${DEBUGGING}
# Run both scripts
cd ../benchmark_scripts
# Run benchmark
OUTPUT_FILE_BLAS="blas_petsc.txt"
rm $OUTPUT_FILE_BLAS
touch $OUTPUT_FILE_BLAS
# Run blas benchmark
#${RUN_COMMAND} -n 1 ../src/petsc_blas | tee $OUTPUT_FILE_BLAS
# Run scripts
OUTPUT_FILE_CORES="cores_petsc.txt"
OUTPUT_FILE_DATA="data_petsc.txt"
rm $OUTPUT_FILE_CORES
rm $OUTPUT_FILE_DATA
LOOP=5
# Run cores_script from 1 to 256 cores
START=1
END=128
STEP=2
N_TRAIN=20000
N_TEST=5000
N_REG=100
#./cores_script.sh $START $END $STEP $N_TRAIN $N_TEST $N_REG $LOOP $OUTPUT_FILE_CORES| tee -a $OUTPUT_FILE_CORES
# Run data_script from 1 000 to 9 000
START=1000
END=9000
STEP=1000
N_CORES=128
N_TEST=5000
N_REG=100
#./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA
# Run data_script from 10 000 to 100 000
START=10000
END=100000
STEP=10000
N_CORES=128
N_TEST=5000
N_REG=100
#./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA
# Run data_script for testing
START=10000
END=10000
STEP=1000
N_CORES=8
N_TEST=5000
N_REG=100
./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP $OUTPUT_FILE_DATA| tee -a $OUTPUT_FILE_DATA
