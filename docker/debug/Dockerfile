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
# configure  and compile petsc in debug mode
RUN cd petsc && ./configure --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# export petsc
RUN export PETSC_DIR=$HOME/petsc && export PETSC_ARCH=petsc-arch

# remove docker error message (somehow does not work and needs to be done manually)
# RUN export OMPI_MCA_btl_vader_single_copy_mechanism=none
