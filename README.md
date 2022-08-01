# PETSc Project

description tbd.

## Release

### Release Docker Container

Build image inside petsc_project folder:

`sudo docker build . -f docker/release/Dockerfile -t petsc_release_image`

Start container:

`sudo docker run -it --rm --user user --name petsc_release_container petsc_release_image`

### Inside Release Container
Compile Code:

`cd && cd petsc_project && git pull --rebase && cd src && make petsc_cholesky DEBUG=0 && export OMPI_MCA_btl_vader_single_copy_mechanism=none`

Run Code:

`cd && cd petsc_project/src && mpirun -n 6 petsc_cholesky -n_train 100000 -n_test 5000 -n_regressors 100`


## Debug

### Debug Docker Container

Build image inside petsc_project folder:

`sudo docker build . -f docker/debug/Dockerfile -t petsc_debug_image`

Run container inside petsc_project folder:

`sudo docker run -it --rm --user user --name petsc_debug_container petsc_debug_image`

### Inside Debug Container

Compile Code:

`cd && cd petsc_project && git pull --rebase && cd src && make petsc_cholesky DEBUG=1 && export OMPI_MCA_btl_vader_single_copy_mechanism=none`

Run Code:

`cd && cd petsc_project/src && mpirun -n 6 petsc_cholesky -n_train 100000 -n_test 5000 -n_regressors 100`

## Scripts

MPI Node Loop:

`cd && cd petsc_project/scripts && chmod +x cores_script.sh && ./cores_script.sh 1 6 1 1000 1000 100 | tee -a cores_result.txt`

Training Size Loop:

`cd && cd petsc_project/scripts && chmod +x data_script.sh && ./data_script.sh 1000 5000 1000 6 5000 100 | tee -a data_result.txt`

Benchmark Script:

`cd && cd petsc_project && git pull --rebase && chmod +x run_benchmarks.sh && ./run_benchmarks.sh`


## Git Commands

`git add . && git commit -m "comment" && git push`

`git add . && git commit --amend --no-edit && git push -f`
