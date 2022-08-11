#!/bin/bash
: ${SOURCE_ROOT:?} ${INSTALL_ROOT:?} ${PYTHON_VERSION:?}

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
