#include <stdio.h>
#include <petscksp.h>
typedef double real;
#define type "%lf"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  // Load data
  FILE    *training_input_file;
  FILE    *training_output_file;
  FILE    *test_input_file;
  FILE    *test_output_file;

  real    value;
  const int length_training = 100 * 1000;
  const int length_test = 5 * 1000;

  float   training_input[length_training];
  float   training_output[length_training];
  float   test_input[length_test];
  float   test_output[length_test];

  training_input_file = fopen("data/training/training_input.txt", "r");
  training_output_file = fopen("data/training/training_output.txt", "r");
  test_input_file = fopen("data/test/test_input_3.txt", "r");
  test_output_file = fopen("data/test/test_output_3.txt", "r");

  if (training_input_file == NULL || training_output_file == NULL || test_input_file == NULL || test_output_file == NULL)
  {
    printf("return 1\n");
    return 1;
  }
  // load training data
  for (int i = 0; i < length_training; i++)
  {
    fscanf(training_input_file,type,&value);
    training_input[i] = value;
    fscanf(training_output_file,type,&value);
    training_output[i] = value;
  }
  // load test data
  for (int i = 0; i < length_test; i++)
  {
    fscanf(test_input_file,type,&value);
    test_input[i] = value;
    fscanf(test_output_file,type,&value);
    test_output[i] = value;
  }
  // close file streams
  fclose(training_input_file);
  fclose(training_output_file);
  fclose(test_input_file);
  fclose(test_output_file);

  // Petsc
  Vec            u_train,y_train;    // training_input, training_output
  Vec            u_test,y_test;      // test_input, test_input
  Mat            R;                  // regressor matrix
  Mat            K;                  // covariance matrix
  PetscErrorCode ierr;
  PetscMPIInt    size;
  PetscInt       n_training = length_training, n_test = length_test;
  PetscInt       i,n_regressors;
  PetscScalar    u_i,u_j;

  ierr = PetscInitialize(&argc,&args,(char*)0,PETSC_NULL);if (ierr) return ierr;
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  if (size != 1) SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_WRONG_MPI_SIZE,"This is a uniprocessor example only!");
  ierr = PetscOptionsGetInt(NULL,NULL,"-n",&n_training,NULL);CHKERRQ(ierr);
  /*
     Create vectors.  Note that we form 2 vector from scratch and
     then duplicate as needed.
  */
  // Create train vectors
  ierr = VecCreate(PETSC_COMM_WORLD,&u_train);CHKERRQ(ierr);
  ierr = VecSetSizes(u_train,PETSC_DECIDE,n_training);CHKERRQ(ierr);
  ierr = VecSetFromOptions(u_train);CHKERRQ(ierr);
  ierr = VecDuplicate(u_train,&y_train);CHKERRQ(ierr);
  // Create test vectors
  ierr = VecCreate(PETSC_COMM_WORLD,&u_test);CHKERRQ(ierr);
  ierr = VecSetSizes(u_test,PETSC_DECIDE,n_test);CHKERRQ(ierr);
  ierr = VecSetFromOptions(u_test);CHKERRQ(ierr);
  ierr = VecDuplicate(u_test,&y_test);CHKERRQ(ierr);
  /*
     Create matrix.  When using MatCreate(), the matrix format can
     be specified at runtime.
  */
  // Create regressor matrix
  ierr = MatCreate(PETSC_COMM_WORLD,&R);CHKERRQ(ierr);
  ierr = MatSetSizes(R,PETSC_DECIDE,PETSC_DECIDE, n_regressors,n_training);CHKERRQ(ierr);
  ierr = MatSetFromOptions(R);CHKERRQ(ierr);
  ierr = MatSetUp(R);CHKERRQ(ierr);
  // Create covariance matrix
  ierr = MatCreate(PETSC_COMM_WORLD,&K);CHKERRQ(ierr);
  ierr = MatSetSizes(K,PETSC_DECIDE,PETSC_DECIDE,n_training,n_training);CHKERRQ(ierr);
  ierr = MatSetFromOptions(K);CHKERRQ(ierr);
  ierr = MatSetUp(K);CHKERRQ(ierr);
  // for latter: MatCreateSBAIJ
  /*
     Assemble matrix covariance with covariance matrix
  */
  /*
  for (i = 1; i < length_training; i++)
  {
    for (j = 1; j < length_training; j++)
    {

    }
  }
  value[0] = -1.0; value[1] = 2.0; value[2] = -1.0;

  for (i=1; i<n-1; i++) {
    col[0] = i-1;
    col[1] = i;
    col[2] = i+1;
    ierr   = MatSetValues(A,1,&i,3,col,value,INSERT_VALUES);CHKERRQ(ierr);
  }

  i    = n - 1;
  col[0] = n - 2;
  col[1] = n - 1;
  ierr = MatSetValues(A,1,&i,2,col,value,INSERT_VALUES);CHKERRQ(ierr);
  i    = 0;
  col[0] = 0;
  col[1] = 1;
  value[0] = 2.0;
  value[1] = -1.0;
  ierr = MatSetValues(A,1,&i,2,col,value,INSERT_VALUES);CHKERRQ(ierr);

  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  */


  ierr = PetscFinalize(); CHKERRQ(ierr);

  printf("return 0\n");
  return 0;
}
