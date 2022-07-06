# HOW TO
# Build image:
# sudo docker build . -f docker/Dockerfile -t petsc_image
# Run container inside petsc_project folder:
# sudo docker run -it --rm --user user --mount type=bind,source="$(pwd)",target=/usr/src/petsc_project --name petsc_container petsc_image

FROM ubuntu:22.04

SHELL ["/bin/bash","-c"]
# install essentials
RUN apt-get update -y && apt-get install -y vim make git python3 gcc g++ gfortran

RUN useradd -ms /bin/bash user
USER user
WORKDIR /home/user/petsc_project

COPY . .

# install petsc
RUN cd .. && git clone -b release https://gitlab.com/petsc/petsc.git/ && cd petsc && git pull
# configure  and compile petsc
RUN cd .. && cd petsc && ./configure --download-openmpi --download-fblaslapack && make all check
# export petsc
RUN export PETSC_DIR=$HOME/petsc && export PETSC_ARCH=petsc-arch
