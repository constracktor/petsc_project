export INSTALL_ROOT=${PETSC_ROOT}/build
export SOURCE_ROOT=${PETSC_ROOT}/src

################################################################################
# Package Configuration
################################################################################
# GCC - not used
export GCC_VERSION=10.3.0

# MKL version 
export MKL_VERSION=2023.0.0

# Python
export PYTHON_VERSION=3.10.6

# PETSc
export PETSC_VERSION=3.17.4

# Max number of parallel jobs
export PARALLEL_BUILD=$(grep -c ^processor /proc/cpuinfo)
