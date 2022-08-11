FROM ubuntu:22.04

SHELL ["/bin/bash","-c"]

# install essentials
RUN apt-get update -y && apt-get install -y git build-essential python3

# install dev tools
# RUN apt-get install -y vim

# add user
RUN useradd -ms /bin/bash user
USER user
# create working directory
WORKDIR /home/user/
# clone repository
RUN git clone https://github.com/constracktor/petsc_project.git

RUN cd petsc_project/dependencies && ./build-petsc.sh





# install petsc
#RUN git clone -b release https://gitlab.com/petsc/petsc.git/ && cd petsc && git pull
# configure  and compile petsc in release mode
#RUN cd petsc && ./configure --with-debugging=0 --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# export petsc
#RUN export PETSC_DIR=$HOME/petsc && export PETSC_ARCH=petsc-arch

./configure --with-debugging=0 --with-cc=gcc --with-fc=0 --with-cxx=g++ --download-openmpi --download-f2cblaslapack --download-blis --download-cmake --download-metis --download-parmetis --download-elemental && make all check
