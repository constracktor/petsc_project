-include /home/user/petsc/petscdir.mk

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

petsc_cholesky: petsc_cholesky.o chkopts
	-${CLINKER} -o petsc_cholesky petsc_cholesky.o ${PETSC_LIB}
	${RM} petsc_cholesky.o
