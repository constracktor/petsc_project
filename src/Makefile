PETSC_DIR = $(PETSC_HOME)
PETSC_ARCH = bgq-power
ALL: hello_p
CFLAGS = -g -O0
CPPFLAGS =
CLEANFILES = hello_p

include ${PETSC_DIR}/conf/variables
include ${PETSC_DIR}/conf/rules

hello_p: hello_p.o chkopts
	-${CLINKER} -o hello_p hello_p.o ${PETSC_LIB}
	${RM} hello_p.o
