ALL: hello_p
CFLAGS = -g -O0
CPPFLAGS =
CLEANFILES = hello_p

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

hello_p: hello_p.o chkopts
	-${CLINKER} -o hello_p hello_p.o ${PETSC_LIB}
	${RM} hello_p.o
