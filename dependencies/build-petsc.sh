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
#${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# configure to use f2cblaslapack for BLAS and LAPACK without fortran
#${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=0 --with-cxx=g++ --with-clanguage=cxx --download-openmpi --download-f2cblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# configure to use MKL for BLAS and LAPACK
#${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --download-openmpi --with-blas-lapack-dir=${INSTALL_ROOT}/mkl/mkl/2023.0.0/lib/intel64 --download-elemental --download-metis --download-parmetis --download-cmake && make all check
# NOTE: install on ipvsmisc not ipvs-epyc1
# configure to use fblaslapack for BLAS and LAPACK and own OpenMPI
${PYTHONPATH} configure --with-debugging=${DEBUGGING} --with-cc=gcc --with-fc=gfortran --with-cxx=g++ --with-clanguage=cxx --with-openmpi=$HOME/build/bin --download-fblaslapack --download-elemental --download-metis --download-parmetis --download-cmake && make all check