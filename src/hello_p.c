#include “petsc.h”
int main( int argc, char *argv[] )
{
  PetscInitialize(&argc,&argv,PETSC_NULL,PETSC_NULL);
  PetscPrintf(PETSC_COMM_WORLD,”Hello World\n”);
  PetscFinalize();
  return 0;
}
