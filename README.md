# PETSc Project

description tbd.

## Docker Containers

### Release (PETSc installed in Release mode)

Build image inside petsc_project folder:

`sudo docker build . -f docker/release/Dockerfile -t petsc_release_image`

Start container:

`sudo docker run -it --rm --user user petsc_release_image`


### Debug (PETSc installed in Debug mode)

Build image inside petsc_project folder:

`sudo docker build . -f docker/debug/Dockerfile -t petsc_debug_image`

Run container:

`sudo docker run -it --rm --user user petsc_debug_image`


### Base (Install PETSc manually)

Build image inside petsc_project folder:

`sudo docker build . -f docker/base/Dockerfile -t petsc_base_image`

Run container:

`sudo docker run -it --rm --user user petsc_base_image`

Install PETSc manually:

`cd && cd petsc_project/dependencies && git pull --rebase && ./build-all.sh Release`


## Compile Code Manually

### Release

Compile Code:

`cd && cd petsc_project && git pull --rebase && cd src && make petsc_cholesky DEBUG=0 && export OMPI_MCA_btl_vader_single_copy_mechanism=none`


### Debug

Compile Code:

`cd && cd petsc_project && git pull --rebase && cd src && make petsc_cholesky DEBUG=1 && export OMPI_MCA_btl_vader_single_copy_mechanism=none`


## Run Code Manually

Run Code:

`cd && cd petsc_project/src && ~/petsc_project/dependencies/src/petsc/lib/petsc/bin/petscmpiexec -n 6 petsc_cholesky -n_train 100000 -n_test 5000 -n_regressors 100`


## Scripts

MPI Node Loop:

`cd && cd petsc_project/scripts && ./cores_script.sh 1 6 1 1000 1000 100 5 | tee -a cores_result.txt`

Training Size Loop:

`cd && cd petsc_project/scripts && ./data_script.sh 1000 5000 1000 6 5000 100 5 | tee -a data_result.txt`

Benchmark Script:

`cd && cd petsc_project && git pull --rebase && ./run_benchmarks.sh Release`


## Git Commands

`git add . && git commit -m "comment" && git push`

`git add . && git commit --amend --no-edit && git push -f`
