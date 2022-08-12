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
cd ../benchmark_scripts && rm cores_result.txt && rm data_result.txt
LOOP=5
# Run cores_script
START=1
END=6
STEP=1
N_TRAIN=1000
N_TEST=1000
N_REG=100
./cores_script.sh $START $END $STEP $N_TRAIN $N_TEST $N_REG $LOOP| tee -a cores_result.txt
# Run data_script
START=1000
END=5000
STEP=1000
N_CORES=6
N_TEST=1000
N_REG=100
./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG $LOOP| tee -a data_result.txt
