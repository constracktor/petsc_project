#!/bin/bash
# Get from sourceforge
#DIR_SRC="python"

#DOWNLOAD_URL="https://www.python.org/ftp/python/3.10.6/Python-3.10.6.tar.xz"
#mkdir -p ${DIR_SRC}
#cd ${DIR_SRC}

#wget -O- ${DOWNLOAD_URL} | tar xJ --strip-components=1

#mkdir build
#./configure --prefix=$(pwd)/build
#make && make install

: ${SOURCE_ROOT:?} ${INSTALL_ROOT:?} ${GCC_VERSION:?}

DIR_SRC=${SOURCE_ROOT}/python
DIR_BUILD=${INSTALL_ROOT}/python/build
DIR_INSTALL=${INSTALL_ROOT}/python

DOWNLOAD_URL="https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tar.xz"

if [[ ! -d ${DIR_SRC} ]]; then
    (
        mkdir -p ${DIR_SRC}
        cd ${DIR_SRC}
        wget -O- ${DOWNLOAD_URL} | tar xJ --strip-components=1
    )
fi

(
    mkdir -p ${DIR_BUILD}
    cd ${DIR_BUILD}

    ${DIR_SRC}/configure --prefix=${DIR_INSTALL}
    make -j${PARALLEL_BUILD}
    make install
)
