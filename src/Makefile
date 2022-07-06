include ../../../../petscdir.mk

CFLAGS             =
FFLAGS             =
CPPFLAGS           =
FPPFLAGS           =
LOCDIR             =   /home/user/petsc_project/src
EXAMPLESC          =   hello_p.c
EXAMPLESF          =
MANSEC             =
CLEANFILES         =
NP                 =

include ${PETSC_DIR}/petsc/conf/variables
include ${PETSC_DIR}/petsc/conf/rules
include ${PETSC_DIR}/petsc/conf/test

hello_p: hello_p.o  chkopts
	-${CLINKER} -o hello_p hello_p.o  ${PETSC_SYS_LIB}
	${RM} hello_p.o
