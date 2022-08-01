#!/bin/bash
# Compile Code
cd && cd petsc_project/src && make petsc_cholesky DEBUG=0 && export OMPI_MCA_btl_vader_single_copy_mechanism=none
# Run cores_script
START=1
END=6
STEP=1
N_TRAIN=1000
N_TEST=1000
N_REG=100
cd && cd petsc_project/scripts && chmod +x cores_script.sh && ./cores_script.sh $START $END $STEP $N_TRAIN $N_TEST $N_REG | tee -a cores_result.txt
# Run data_script
START=1000
END=5000
STEP=1000
N_CORES=6
N_TEST=1000
N_REG=100
cd && cd petsc_project/scripts && chmod +x data_script.sh && ./data_script.sh $START $END $STEP $N_CORES $N_TEST $N_REG | tee -a data_result.txt
