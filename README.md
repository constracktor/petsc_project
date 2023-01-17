# PETSc Project

Repository that contains the PETSc implementation of the non-linear system
identification with Gaussian processes minimum working example used for
"Scalability of Gaussian Processes using Asynchronous Tasks:
A Comparison between HPX and PETSc"

## Docker Containers

### Release (PETSc installed in Release mode)

Build image inside petsc_project folder:  
`sudo docker build . -f docker/Dockerfile_release -t petsc_release_image`  
Start container:  
`sudo docker run -it --rm --user user petsc_release_image`  

### Debug (PETSc installed in Debug mode)

Build image inside petsc_project folder:  
`sudo docker build . -f docker/Dockerfile_debug -t petsc_debug_image`  
Start container:  
`sudo docker run -it --rm --user user petsc_debug_image`  

### Base (Install PETSc manually)

Build image inside petsc_project folder:  
`sudo docker build . -f docker/Dockerfile_base -t petsc_base_image`  
Start container:  
`sudo docker run -it --rm --user user petsc_base_image`  
Install PETSc manually:  
`cd && cd petsc_project/dependencies && git pull --rebase && ./build-all.sh Release`  

## Compile and Run Benchmark

Benchmark Script:  
`cd && cd petsc_project && git pull --rebase && ./run_benchmarks.sh release/debug cpu/blas`  

## Git Commands for Developing

`git add . && git commit -m "comment" && git push`  
`git add . && git commit --amend --no-edit && git push -f`  
