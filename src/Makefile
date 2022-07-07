-include /home/user/petsc/petscdir.mk

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

hello_p: hello_p.o chkopts
	-${CLINKER} -o hello_p hello_p.o ${PETSC_LIB}
	${RM} hello_p.o

sle_solver: sle_solver.o chkopts
	-${CLINKER} -o sle_solver sle_solver.o ${PETSC_LIB}
	${RM} sle_solver.o
