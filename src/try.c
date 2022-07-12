#include <stdio.h>
#include <petscksp.h>
#define type "%lf"

#undef __FUNCT__
#define __FUNCT__ "main"

void standardize(PetscInt array_size, PetscScalar *array)//needs later to return mean and std
{
  // compute mean
  PetscScalar mean = 0.0;
  for (PetscInt i = 0; i < array_size; i++)
  {
    mean += array[i];
  }
  mean = mean / array_size;
  // compute standard deviation
  PetscScalar standard_deviation = 0.0;
  for(PetscInt i = 0; i < array_size; i++)
  {
    standard_deviation += PetscPowReal(array[i] - mean,2);
  }
  standard_deviation = PetscSqrtReal(standard_deviation / array_size);
  // standardize input array
  for(PetscInt i = 0; i < array_size; i++)
  {
    array[i] = (array[i] - mean) / standard_deviation;
  }
}

void compute_regressor_vector(PetscInt row, PetscInt n_regressors, PetscScalar *training_input, PetscScalar *u_row )
{
   for (PetscInt i = 0; i < n_regressors; i++)
   {
     PetscInt index = row - n_regressors + 1 + i;
     if (index < 0)
     {
        u_row[i] = (PetscScalar)0.0;
     }
     else
     {
       u_row[i] = training_input[index];
     }
   }
}

PetscScalar compute_covariance_fuction(PetscInt n_regressors, PetscScalar *z_i, PetscScalar *z_j, PetscScalar *hyperparameters)
{
  // Compute the Squared Exponential Covariance Function
  // C(z_i,z_j) = vertical_lengthscale * exp(-0.5*lengthscale*(z_i-z_j)^2)
  PetscScalar distance = 0.0;
  for (PetscInt i = 0; i < n_regressors; i++)
  {
    distance += PetscPowReal(z_i[i] - z_j[i],2);
  }
  return hyperparameters[1] * PetscExpReal(-0.5 * hyperparameters[0] * distance);
}

int main(int argc,char **args)
{ // parameters
  PetscInt       n_training = 100;//1 * 1000;//max 100*1000
  PetscInt       n_test = 1 * 1000;
  PetscInt       n_regressors = 100;
  PetscInt       i,j;
  PetscScalar    value;
  PetscErrorCode ierr;
  PetscMPIInt    size;
  // Petsc structures
  Vec            y_train;            // training_output
  Vec            cross_covariance;   // cross_covariance
  Mat            K;                  // covariance matrix
  Mat            L;                  // Cholesky decomposition
  // GP hyperparameters
  // hyperparameters[0] = lengthscale
  // hyperparameters[1] = vertical_lengthscale
  // hyperparameters[2] = noise_variance
  PetscScalar hyperparameters[3];
  // data holders
  PetscScalar   training_input[n_training];
  PetscScalar   training_output[n_training];
  PetscScalar   test_input[n_test];
  PetscScalar   test_output[n_test];
  // data files
  FILE    *training_input_file;
  FILE    *training_output_file;
  FILE    *test_input_file;
  FILE    *test_output_file;
  // Petsc initialization
  ierr = PetscInitialize(&argc,&args,(char*)0,PETSC_NULL);if (ierr) return ierr;
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  if (size != 1)
  {
    SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_WRONG_MPI_SIZE,"This is a uniprocessor example only!");
  }
  //////////////////////////////////////////////////////////////////////////////
  // loadtraining and test data from .txt files
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
  for (i = 0; i < n_training; i++)
  {
    fscanf(training_input_file,type,&value);
    training_input[i] = value;
    fscanf(training_output_file,type,&value);
    training_output[i] = value;
  }
  // load test data
  for (i = 0; i < n_test; i++)
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
  //////////////////////////////////////////////////////////////////////////////
  // standardize data for stability
  standardize(n_training, training_input);
  standardize(n_training, training_output);
  // initalize hyperparameters to empirical moments of the data
  hyperparameters[0] = 1.0;// variance of training_output
  hyperparameters[1] = 1.0;// standard deviation of training_input
  hyperparameters[2] = 0.001; // experience?
  //////////////////////////////////////////////////////////////////////////////
  // Create Petsc structures
  // Create vectors
  ierr = VecCreate(PETSC_COMM_WORLD,&y_train);CHKERRQ(ierr);
  ierr = VecSetSizes(y_train,PETSC_DECIDE,n_training);CHKERRQ(ierr);
  ierr = VecSetFromOptions(y_train);CHKERRQ(ierr);
  ierr = VecDuplicate(y_train,&cross_covariance);CHKERRQ(ierr);
  // Create matrix.  When using MatCreate(), the matrix format can
  // be specified at runtime.
  // Create covariance matrix
  ierr = MatCreate(PETSC_COMM_WORLD,&K);CHKERRQ(ierr);
  ierr = MatSetSizes(K,PETSC_DECIDE,PETSC_DECIDE,n_training,n_training);CHKERRQ(ierr);
  ierr = MatSetFromOptions(K);CHKERRQ(ierr);
  ierr = MatSetUp(K);CHKERRQ(ierr);
  ierr = MatDuplicate(K,MAT_DO_NOT_COPY_VALUES,&L);
  /*
  // Create Cholesky matrix
  ierr = MatCreate(PETSC_COMM_WORLD,&L);CHKERRQ(ierr);
  ierr = MatSetSizes(L,PETSC_DECIDE,PETSC_DECIDE,n_training,n_training);CHKERRQ(ierr);
  ierr = MatSetFromOptions(L);CHKERRQ(ierr);
  ierr = MatSetUp(L);CHKERRQ(ierr);
  */
  //////////////////////////////////////////////////////////////////////////////
  // Assemble Petsc structures
  // Assemble output vector
  for (i = 0; i < n_training; i++)
  {
    // y_train contains the training output
    VecSetValues(y_train,1,&i,&training_output[i],INSERT_VALUES);CHKERRQ(ierr);
  }
  // Assemble covariance matrix
  for (i = 0; i < n_training; i++)
  {
    for (j = 0; j < n_training; j++)
    {
      // compute regressor vectors
      PetscScalar z_i[n_regressors];
      compute_regressor_vector(i, n_regressors, training_input, z_i);
      PetscScalar z_j[n_regressors];
      compute_regressor_vector(j, n_regressors, training_input, z_j);
      // compute covariance function
      PetscScalar covariance_function = compute_covariance_fuction(n_regressors, z_i, z_j, hyperparameters);
      // add noise_variance on diagonal
      if (i==j)
      {
        covariance_function += hyperparameters[2];
      }
      // write covariance function value to covariance matrix
      ierr = MatSetValues(K,1,&i,1,&j,&covariance_function,INSERT_VALUES);CHKERRQ(ierr);

      if (j == i)
      {
        printf("\n%lf\n\n", covariance_function);
      }

    }
  }
  ierr = MatAssemblyBegin(K,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(K,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  // print matrix
  ierr = MatView(K,PETSC_VIEWER_STDOUT_(PETSC_COMM_WORLD));
  //////////////////////////////////////////////////////////////////////////////
  // Compute cholesky decompostion of K
  //KSP            ksp;          /* linear solver context */
  PC             pc;           /* preconditioner context */
  //ierr = KSPCreate(PETSC_COMM_WORLD,&ksp);CHKERRQ(ierr);
  //ierr = KSPSetOperators(ksp,K,K);CHKERRQ(ierr);
  //ierr = KSPGetPC(ksp,&pc);CHKERRQ(ierr);
  //ierr = PCSetType(pc,PCCHOLESKY);CHKERRQ(ierr);
  //ierr = KSPSetTolerances(ksp,1.e-5,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);CHKERRQ(ierr);

  //ierr = KSPSetFromOptions(ksp);CHKERRQ(ierr);
  //ierr = PCFactorGetMatrix(pc,&L)CHKERRQ(ierr);;


  ierr = PCCreate(PETSC_COMM_WORLD,&pc);CHKERRQ(ierr);
  ierr = PCSetOperators(pc,K,K);CHKERRQ(ierr);
  ierr = PCSetType(pc,PCCHOLESKY);CHKERRQ(ierr);
  ierr = PCSetUp(pc);CHKERRQ(ierr);
  ierr = PCFactorGetMatrix(pc,&L);CHKERRQ(ierr);
  //////////////////////////////////////////////////////////////////////////////
  // Make predictions
/*
// cross_covariance
PetscScalar prediction_input[n_regressors];
compute_regressor_vector(100, n_regressors, test_input, prediction_input)
if(i=100)
VecSetValues(cross_covariance,1,&i,u_i,INSERT_VALUES);CHKERRQ(ierr);
*/



  printf("Terminated");
  // finalize Petsc
  ierr = PetscFinalize(); CHKERRQ(ierr);
  return ierr;
}
