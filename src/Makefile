-include /home/user/petsc/petscdir.mk

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

hello_p: hello_p.o chkopts
	-${CLINKER} -o hello_p hello_p.o ${PETSC_LIB}
	${RM} hello_p.o

parallel_sle_solver: parallel_sle_solver.o chkopts
	-${CLINKER} -o parallel_sle_solver parallel_sle_solver.o ${PETSC_LIB}
	${RM} parallel_sle_solver.o
