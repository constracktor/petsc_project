# HOW TO
# Build image:
# sudo docker build . -f docker/Dockerfile -t petsc_image
#
# Run container inside petsc_project folder:
# sudo docker run -it --rm --user user --name petsc_container petsc_image
#
# Run benchmark script
# cd && cd petsc_project && git pull --rebase && chmod +x run_benchmarks.sh && ./run_benchmarks.sh
#
# Compile Code:
# cd && cd petsc_project && git pull --rebase && cd src && make petsc_cholesky && export OMPI_MCA_btl_vader_single_copy_mechanism=none
#
# Run Code:
# cd && cd petsc_project/src && mpirun -n 6 petsc_cholesky -n_train 100000 -n_test 5000 -n_regressors 100
#
# Run Script:
# cd && cd petsc_project/scripts && chmod +x cores_script.sh && ./cores_script.sh 1 6 1 1000 1000 100 | tee -a cores_result.txt
#
# Update Repository:
# git add . && git commit -m "comment" && git push
# git add . && git commit --amend --no-edit && git push -f

FROM ubuntu:22.04

SHELL ["/bin/bash","-c"]
# install essentials
RUN apt-get update -y && apt-get install -y vim make git python3 gcc g++ gfortran openmpi-bin

# add user
RUN useradd -ms /bin/bash user
USER user
# create working directory
WORKDIR /home/user/
# clone repository
RUN git clone https://github.com/constracktor/petsc_project.git

# install petsc
RUN git clone -b release https://gitlab.com/petsc/petsc.git/ && cd petsc && git pull
# configure  and compile petsc
RUN cd petsc && ./configure --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# export petsc
RUN export PETSC_DIR=$HOME/petsc && export PETSC_ARCH=petsc-arch

# remove docker error message (somehow does not work and needs to be done manually)
RUN export OMPI_MCA_btl_vader_single_copy_mechanism=none
