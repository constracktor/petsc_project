#include <stdio.h>
#include <petscksp.h>
#define type "%lf"

#undef __FUNCT__
#define __FUNCT__ "main"

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
{ // GP parameters
  PetscInt       n_train = 100 * 1000;  //max 100*1000
  PetscInt       n_test = 5 * 1000;     //max 5*1000
  PetscInt       n_regressors = 100;
  PetscScalar    hyperparameters[3];
  // initalize hyperparameters to empirical moments of the data
  hyperparameters[0] = 1.0;   // lengthscale = variance of training_output
  hyperparameters[1] = 1.0;   // vertical_lengthscale = standard deviation of training_input
  hyperparameters[2] = 0.001; // noise_variance = small value
  // Petsc structures
  PetscInt       n_cores;
  PetscInt       i,j;
  PetscInt       rstart_train,rend_train,n_train_local;
  PetscInt       rstart_test,rend_test,n_test_local;
  PetscInt       indices[n_train];
  PetscReal      covariance_function, error;
  PetscScalar    z_i[n_regressors], z_j[n_regressors];
  Vec            y_train,alpha;         // training_output; alpha = K^-1 * y_train; L*beta=y_train
  Vec            y_test,test_prediction;// test_output
  Mat            K;                     // covariance matrix
  Mat            L;                     // Cholesky decomposition
  Mat            cross_covariance;      // cross covariance of all test samples
  KSP            ksp;          // linear solver context
  PC             pc;           // preconditioner context
  // data holders for assembly
  PetscScalar   training_input[n_train];
  PetscScalar   training_output[n_train];
  PetscScalar   test_input[n_test];
  PetscScalar   test_output[n_test];
  // data files
  FILE    *training_input_file;
  FILE    *training_output_file;
  FILE    *test_input_file;
  FILE    *test_output_file;
  // Petsc initialization
  PetscCall(PetscInitialize(&argc,&args,(char*)0,PETSC_NULL));
  // Get parameters
  PetscCallMPI(MPI_Comm_size(PETSC_COMM_WORLD,&n_cores));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_train",&n_train,NULL));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_test",&n_test,NULL));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_regressors",&n_regressors,NULL));
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
  for (i = 0; i < n_train; i++)
  {
    fscanf(training_input_file,type,&training_input[i]);
    fscanf(training_output_file,type,&training_output[i]);
  }
  // load test data
  for (i = 0; i < n_test; i++)
  {
    fscanf(test_input_file,type,&test_input[i]);
    fscanf(test_output_file,type,&test_output[i]);
  }
  // close file streams
  fclose(training_input_file);
  fclose(training_output_file);
  fclose(test_input_file);
  fclose(test_output_file);
  //////////////////////////////////////////////////////////////////////////////
  // Create Petsc structures
  // Create vector y_train
  PetscCall(VecCreate(PETSC_COMM_WORLD,&y_train));
  PetscCall(VecSetSizes(y_train,PETSC_DECIDE,n_train));
  PetscCall(VecSetFromOptions(y_train));
  // Duplicate vector alpha
  PetscCall(VecDuplicate(y_train,&alpha));
  // Create vector test_prediction
  PetscCall(VecCreate(PETSC_COMM_WORLD,&y_test));
  PetscCall(VecSetSizes(y_test,PETSC_DECIDE,n_test));
  PetscCall(VecSetFromOptions(y_test));
  // Duplicate vector test_prediction
  PetscCall(VecDuplicate(y_test,&test_prediction));
  // Identify starting and ending points (We let PETSc decide above)
  PetscCall(VecGetOwnershipRange(y_train,&rstart_train,&rend_train));
  PetscCall(VecGetLocalSize(y_train,&n_train_local));
  PetscCall(VecGetOwnershipRange(y_test,&rstart_test,&rend_test));
  PetscCall(VecGetLocalSize(y_test,&n_test_local));

  // Create matrix.  When using MatCreate(), the matrix format can
  // be specified at runtime.
  // Create covariance matrix
  PetscCall(MatCreate(PETSC_COMM_WORLD,&K));
  PetscCall(MatSetType(K, MATMPIDENSE));
  PetscCall(MatSetSizes(K,n_train_local,PETSC_DECIDE,n_train,n_train));
  PetscCall(MatSetFromOptions(K));
  PetscCall(MatSetUp(K));
  // Create Cholesky matrix
  PetscCall(MatDuplicate(K,MAT_DO_NOT_COPY_VALUES,&L));
  // Create cross covariance matrix
  PetscCall(MatCreate(PETSC_COMM_WORLD,&cross_covariance));
  PetscCall(MatSetType(cross_covariance, MATMPIDENSE));
  PetscCall(MatSetSizes(cross_covariance,n_test_local,PETSC_DECIDE,n_test,n_train));
  PetscCall(MatSetFromOptions(cross_covariance));
  PetscCall(MatSetUp(cross_covariance));
  //////////////////////////////////////////////////////////////////////////////
  // Assemble Petsc structures
  for (i = 0; i < n_train; i++)
  {
    indices[i] = i;
  }
  // Assemble training output vector
  PetscCall(VecSetValues(y_train,n_train,indices,training_output,INSERT_VALUES));
  PetscCall(VecAssemblyBegin(y_train));
  PetscCall(VecAssemblyEnd(y_train));
  // Assemble test output vector
  PetscCall(VecSetValues(y_test,n_test,indices,test_output,INSERT_VALUES));
  PetscCall(VecAssemblyBegin(y_test));
  PetscCall(VecAssemblyEnd(y_test));
  // Assemble covariance matrix according to chunks of contiguous rows
  for (i = rstart_train; i < rend_train; i++)
  {
    compute_regressor_vector(i, n_regressors, training_input, z_i);
    for (j = 0; j < n_train; j++)
    {
      compute_regressor_vector(j, n_regressors, training_input, z_j);
      // compute covariance function
      covariance_function = compute_covariance_fuction(n_regressors, z_i, z_j, hyperparameters);
      // add noise_variance on diagonal
      if (i==j)
      {
        covariance_function += hyperparameters[2];
      }
      // write covariance function value to covariance matrix
      PetscCall(MatSetValues(K,1,&i,1,&j,&covariance_function,INSERT_VALUES));
    }
  }
  PetscCall(MatAssemblyBegin(K,MAT_FINAL_ASSEMBLY));
  PetscCall(MatAssemblyEnd(K,MAT_FINAL_ASSEMBLY));
  // Assemble cross covariance matrix according to chunks of contiguous rows
  for (i = rstart_test; i < rend_test; i++)
  {
    compute_regressor_vector(i, n_regressors, test_input, z_i);
    for (j = 0; j < n_train; j++)
    {
      compute_regressor_vector(j, n_regressors, training_input, z_j);
      // compute covariance function
      covariance_function = compute_covariance_fuction(n_regressors, z_i, z_j, hyperparameters);
      // write covariance function value to covariance matrix
      PetscCall(MatSetValues(cross_covariance,1,&i,1,&j,&covariance_function,INSERT_VALUES));
    }
  }
  PetscCall(MatAssemblyBegin(cross_covariance,MAT_FINAL_ASSEMBLY));
  PetscCall(MatAssemblyEnd(cross_covariance,MAT_FINAL_ASSEMBLY));
  //////////////////////////////////////////////////////////////////////////////
  // Convert covariance matrix to solver format
  PetscCall(MatConvert(K,MATELEMENTAL,MAT_INPLACE_MATRIX ,&K));
  // Setup cholesky solver
  PetscCall(KSPCreate(PETSC_COMM_WORLD,&ksp));
  PetscCall(KSPSetOperators(ksp,K,K));
  PetscCall(KSPGetPC(ksp,&pc));
  PetscCall(KSPSetType(ksp,KSPPREONLY));
  PetscCall(PCSetType(pc,PCCHOLESKY));
  PetscCall(PCFactorSetMatSolverType(pc,MATSOLVERELEMENTAL));
  PetscCall(KSPSetUp(ksp));
  //////////////////////////////////////////////////////////////////////////////
  // Compute alpha
  PetscCall(KSPSolve(ksp,y_train,alpha));
  // Make predictions
  PetscCall(MatMult(cross_covariance,alpha,test_prediction));
  // Compute euklidian norm between vectors
  PetscCall(VecAXPY(y_test,-1.0,test_prediction));
  PetscCall(VecNorm(y_test,NORM_2,&error));
  //////////////////////////////////////////////////////////////////////////////
  //print stuff
  //PetscCall(VecView(alpha,PETSC_VIEWER_STDOUT_WORLD));
  //PetscCall(VecView(y_test,PETSC_VIEWER_STDOUT_WORLD));
  //PetscCall(VecView(test_prediction,PETSC_VIEWER_STDOUT_WORLD));
  //PetscCall(KSPView(ksp,PETSC_VIEWER_STDOUT_WORLD));
  //PetscCall(MatView(K,PETSC_VIEWER_STDOUT_(PETSC_COMM_WORLD)));
  //PetscCall(MatView(cross_covariance,PETSC_VIEWER_STDOUT_(PETSC_COMM_WORLD)));
  //PetscCall(PetscPrintf(PETSC_COMM_SELF,"Start: %d Stop: %d\n", rstart_train,rend_train));
  //PetscCall(PetscPrintf(PETSC_COMM_WORLD,"Average Error: %lf\n", error / n_test));
  // print output information
  PetscCall(PetscPrintf(PETSC_COMM_WORLD,"%d;time;%lf,%d;%d;%d;\n", n_cores, error / n_test, n_train, n_test, n_regressors));
  // Free work space -> All PETSc objects should be destroyed when they are no longer needed.
  PetscCall(VecDestroy(&y_train));
  PetscCall(VecDestroy(&y_test));
  PetscCall(VecDestroy(&alpha));
  PetscCall(VecDestroy(&test_prediction));
  PetscCall(MatDestroy(&cross_covariance));
  PetscCall(MatDestroy(&K));
  PetscCall(MatDestroy(&L));
  // finalize Petsc
  PetscCall(PetscFinalize());
  return 0;
}
