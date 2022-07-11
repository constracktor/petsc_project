#include <stdio.h>
#include <petscksp.h>
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

  PetscScalar    value;
  PetscInt n_training = 100 * 1000;
  PetscInt n_test = 5 * 1000;

  PetscScalar   training_input[n_training];
  PetscScalar   training_output[n_training];
  PetscScalar   test_input[n_test];
  PetscScalar   test_output[n_test];

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
  for (int i = 0; i < n_training; i++)
  {
    fscanf(training_input_file,type,&value);
    training_input[i] = value;
    fscanf(training_output_file,type,&value);
    training_output[i] = value;
  }
  for (int i = 0; i < n_training; i++)
  {
    printf("%lf", training_input[i]);
  }
  // load test data
  for (int i = 0; i < n_test; i++)
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
  PetscInt       i,j,n_zeros;
  PetscInt       n_regressors = 5;
  PetscScalar    u_i,y_i;

  ierr = PetscInitialize(&argc,&args,(char*)0,PETSC_NULL);if (ierr) return ierr;
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  if (size != 1) SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_WRONG_MPI_SIZE,"This is a uniprocessor example only!");


  //   Create vectors.  Note that we form 2 vector from scratch and
  //   then duplicate as needed.
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

  // Create matrix.  When using MatCreate(), the matrix format can
  // be specified at runtime.

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

  // Assemble vectors

  // Assemble training data
  for (i = n_zeros; i < n_training; i++)
  {
    u_i = training_input[i]
    y_i = training_output[i]

    VecSetValues(u_train,1,&i,u_i,INSERT_VALUES);CHKERRQ(ierr);
    VecSetValues(y_train,1,&i,y_i,INSERT_VALUES);CHKERRQ(ierr);
  }

  // Assemble matrices

  // Assemble regressor matrix
  for (j = 0; j < n_regressors; j++)
  {
    n_zeros = n_regressors - 1 - j;
    // fill first n_zeros entries with zeros
    for (i = 0; i < n_zeros; i++)
    {
      //R[i,j] = 0
      //MatSetValues(Mat mat,PetscInt m,const PetscInt idxm[],PetscInt n,const PetscInt idxn[],const PetscScalar v[],InsertMode addv)
      ierr = MatSetValues(R,1,&i,1,&j,0.0,INSERT_VALUES);CHKERRQ(ierr);
    }
    // fill remaining entries with training_input
    for (i = n_zeros; i < n_training; i++)
    {
      //R[i,j] = u_train[i - n_zeros]
      u_i = training_input[i - n_zeros];
      ierr = MatSetValues(R,1,&i,1,&j,u_i,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = MatAssemblyBegin(R,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(R,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  // Assemble covariance matrix
  for (i = 1; i < n_training; i++)
  {
    for (j = 1; j < n_training; j++)
    {

    }
  }
  */
  /*
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
