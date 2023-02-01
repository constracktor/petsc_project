#!/usr/bin/env bash

################################################################################
# Command-line help
################################################################################
print_usage_abort ()
{
    cat <<EOF >&2
SYNOPSIS
    ${0} {Release|Debug}
DESCRIPTION
    Download, configure, build, and install PETSc and its dependencies.
EOF
    exit 1
}

################################################################################
# Command-line options
################################################################################
if [[ "$1" == "Release" || "$1" == "Debug" ]]
then
    export BUILD_TYPE=$1
    echo "Build Type: ${BUILD_TYPE}"

else
    echo 'Build type must be provided and has to be "Release" or "Debug"' >&2
    print_usage_abort
fi

################################################################################
# Diagnostics
################################################################################
set -e
set -x

################################################################################
# Configuration
################################################################################
# Script directory
export PETSC_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd )"
# Set Build Configuration Parameters
source config.sh

################################################################################
# Create source and installation directories
################################################################################
mkdir -p ${SOURCE_ROOT} ${INSTALL_ROOT}

################################################################################
# Build tools
################################################################################
#echo "Building GCC"
#./build-gcc.sh

echo "Building MKL"
./build-mkl.sh

echo "Building Python"
./build-python.sh
export PYTHONPATH="${INSTALL_ROOT}/python/bin/python3"

################################################################################
# PETSc
################################################################################
echo "Building PETSc"
./build-petsc.sh
