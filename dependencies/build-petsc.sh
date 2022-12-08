#!/bin/bash

: ${SOURCE_ROOT:?} ${INSTALL_ROOT:?} ${PETSC_VERSION:?} ${PYTHONPATH:?} ${BUILD_TYPE:?}

DIR_SRC=${SOURCE_ROOT}/petsc

if [[ "${BUILD_TYPE}" == "Release" ]]
then
    DEBUGGING=0
else
    DEBUGGING=1
fi

DOWNLOAD_URL="https://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-${PETSC_VERSION}.tar.gz"

mkdir -p ${DIR_SRC}
cd ${DIR_SRC}
wget -O- ${DOWNLOAD_URL} | tar xz --strip-components=1

# configure to use fblaslapack for BLAS and LAPACK
${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# configure to use blis for BLAS and f2cblaslapack for LAPACK (currently blis not working: python interpreter not linked)
#${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-blis --download-f2cblaslapack --download-openmpi --download-elemental --download-metis --download-parmetis --download-cmake && make all check
