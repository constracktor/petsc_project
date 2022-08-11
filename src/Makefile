-include ${PETSC_DIR}/petscdir.mk

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
else
    CFLAGS=-DNDEBUG
endif

petsc_cholesky: petsc_cholesky.o chkopts
	-${CLINKER} -O3 -o petsc_cholesky petsc_cholesky.o ${PETSC_LIB} $(CFLAGS)
	${RM} petsc_cholesky.o
