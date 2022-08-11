#!/bin/bash
# download petsc
#git clone -b release https://gitlab.com/petsc/petsc.git/
# configure  and compile petsc in release mode
#cd petsc && git pull && ./configure --with-debugging=0 --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-f2cblaslapack --download-blis --download-elemental --download-metis --download-parmetis --download-cmake && make all check
DIR_SRC="petsc"
DOWNLOAD_URL="https://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-3.17.4.tar.gz"
mkdir -p ${DIR_SRC}
cd ${DIR_SRC}

wget -O- ${DOWNLOAD_URL} | tar xz --strip-components=1
/home/user/petsc_project/dependencies/python/build/bin/python3 ./configure --with-debugging=0 --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-f2cblaslapack --download-blis --download-elemental --download-metis --download-parmetis --download-cmake && make all check
#tar xf petsc-<version number>.tar.g
