FROM ubuntu:22.04

SHELL ["/bin/bash","-c"]

# install essentials
RUN apt-get update -y && apt-get install -y git build-essential wget xz-utils bzip2 libz-dev gfortran

# install dev tools (not necessary to run benchmark script)
# RUN apt-get install -y vim

# add user
RUN useradd -ms /bin/bash user
USER user
# create working directory
WORKDIR /home/user/

# clone repository
RUN git clone https://github.com/constracktor/petsc_project.git

# install dependencies manually with
# cd && cd petsc_project/dependencies && git pull --rebase && ./build-all.sh Release
