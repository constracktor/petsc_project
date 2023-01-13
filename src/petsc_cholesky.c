#include <stdio.h>
#include <petscksp.h>
#include <petsctime.h>
#define TYPE "%lf"

#undef __FUNCT__
#define __FUNCT__ "main"

// compute feature vector z_i
void compute_regressor_vector(PetscInt row,
                              PetscInt n_regressors,
                              PetscScalar *training_input,
                              PetscScalar *u_row )
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

// compute the squared exponential kernel of two feature vectors
PetscScalar compute_covariance_function(PetscInt n_regressors,
                                        PetscScalar *z_i,
                                        PetscScalar *z_j,
                                        PetscScalar *hyperparameters)
{
  // C(z_i,z_j) = vertical_lengthscale * exp(-0.5*lengthscale*(z_i-z_j)^2)
  PetscScalar distance = 0.0;
  for (PetscInt i = 0; i < n_regressors; i++)
  {
    distance += PetscPowReal(z_i[i] - z_j[i],2);
  }
  return hyperparameters[1] * PetscExpReal(-0.5 * hyperparameters[0] * distance);
}

int main(int argc,char **args)
{
  // Petsc structures
  PetscInt       n_cores,scanned_elements;
  PetscInt       i,j;
  PetscInt       rstart_train,rend_train,n_train_local;
  PetscInt       rstart_test,rend_test,n_test_local;
  PetscInt       indices_train[n_train],indices_test[n_test];;
  PetscReal      covariance_function, error;
  PetscScalar    z_i[n_regressors], z_j[n_regressors];
  PetscLogDouble t_start_assemble,t_stop_assemble,t_start_solve,t_stop_solve,t_start_predict,t_stop_predict;
  Vec            y_train,alpha;         // training_output; alpha = K^-1 * y_train; L*beta=y_train
  Vec            y_test,test_prediction;// test_output; test_prediction
  Mat            K;                     // covariance matrix
  Mat            L;                     // factorized matrix
  Mat            cross_covariance;      // cross-covariance matrix
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
  //////////////////////////////////////////////////////////////////////////////
  // Get and set parameters
  // determine problem size
  PetscInt       n_train = 1 * 1000;  //max 100*1000
  PetscInt       n_test = 1 * 1000;   //max 5*1000
  // GP parameters
  PetscInt       n_regressors = 100;
  PetscScalar    hyperparameters[3];
  // initalize hyperparameters to empirical moments of the data
  hyperparameters[0] = 1.0;   // lengthscale = variance of training_output
  hyperparameters[1] = 1.0;   // vertical_lengthscale = standard deviation of training_input
  hyperparameters[2] = 0.1; // noise_variance = small value
  //////////////////////////////////////////////////////////////////////////////
  // Load data
  training_input_file = fopen("data/training/training_input.txt", "r");
  training_output_file = fopen("data/training/training_output.txt", "r");
  test_input_file = fopen("data/test/test_input.txt", "r");
  test_output_file = fopen("data/test/test_output.txt", "r");
  if (training_input_file == NULL || training_output_file == NULL || test_input_file == NULL || test_output_file == NULL)
  {
    printf("Error in opening data files!\n");
    return 1;
  }
  // load training data
  scanned_elements = 0;
  for (i = 0; i < n_train; i++)
  {
    scanned_elements += fscanf(training_input_file,TYPE,&training_input[i]);
    scanned_elements += fscanf(training_output_file,TYPE,&training_output[i]);
  }
  if (scanned_elements != 2 * n_train)
  {
    printf("Error in reading training data!\n");
    return 1;
  }
  // load test data
  scanned_elements = 0;
  for (i = 0; i < n_test; i++)
  {
    scanned_elements += fscanf(test_input_file,TYPE,&test_input[i]);
    scanned_elements += fscanf(test_output_file,TYPE,&test_output[i]);
  }
  if (scanned_elements != 2 * n_test)
  {
    printf("Error in reading test data!\n");
    return 1;
  }
  // close file streams
  fclose(training_input_file);
  fclose(training_output_file);
  fclose(test_input_file);
  fclose(test_output_file);
  //////////////////////////////////////////////////////////////////////////////
  // PETSc
  // Petsc initialization
  PetscCall(PetscInitialize(&argc,&args,(char*)0,PETSC_NULL));
  // Get parameters
  PetscCallMPI(MPI_Comm_size(PETSC_COMM_WORLD,&n_cores));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_train",&n_train,NULL));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_test",&n_test,NULL));
  PetscCall(PetscOptionsGetInt(NULL,NULL,"-n_regressors",&n_regressors,NULL));
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
  // Create covariance matrix
  PetscCall(MatCreate(PETSC_COMM_WORLD,&K));
  PetscCall(MatSetType(K, MATMPIDENSE));
  PetscCall(MatSetSizes(K,n_train_local,PETSC_DECIDE,n_train,n_train));
  PetscCall(MatSetFromOptions(K));
  PetscCall(MatSetUp(K));
  // Create factorized matrix
  PetscCall(MatDuplicate(K,MAT_DO_NOT_COPY_VALUES,&L));
  // Create cross-covariance matrix
  PetscCall(MatCreate(PETSC_COMM_WORLD,&cross_covariance));
  PetscCall(MatSetType(cross_covariance, MATMPIDENSE));
  PetscCall(MatSetSizes(cross_covariance,n_test_local,PETSC_DECIDE,n_test,n_train));
  PetscCall(MatSetFromOptions(cross_covariance));
  PetscCall(MatSetUp(cross_covariance));
  //////////////////////////////////////////////////////////////////////////////
  // PART 1: ASSEMBLE
  // Start time measurement for assembly
  PetscCall(PetscTime(&t_start_assemble));
  for (i = 0; i < n_train; i++)
  {
    indices_train[i] = i;
  }
  for (i = 0; i < n_test; i++)
  {
    indices_test[i] = i;
  }
  // Assemble training output vector
  PetscCall(VecSetValues(y_train,n_train,indices_train,training_output,INSERT_VALUES));
  PetscCall(VecAssemblyBegin(y_train));
  PetscCall(VecAssemblyEnd(y_train));
  // Assemble test output vector
  PetscCall(VecSetValues(y_test,n_test,indices_test,test_output,INSERT_VALUES));
  PetscCall(VecAssemblyBegin(y_test));
  PetscCall(VecAssemblyEnd(y_test));
  // Assemble covariance matrix according to chunks of contiguous row
  for (i = rstart_train; i < rend_train; i++)
  {
    compute_regressor_vector(i, n_regressors, training_input, z_i);
    for (j = 0; j < n_train; j++)
    {
      compute_regressor_vector(j, n_regressors, training_input, z_j);
      // compute covariance function
      covariance_function = compute_covariance_function(n_regressors, z_i, z_j, hyperparameters);
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
      covariance_function = compute_covariance_function(n_regressors, z_i, z_j, hyperparameters);
      // write covariance function value to covariance matrix
      PetscCall(MatSetValues(cross_covariance,1,&i,1,&j,&covariance_function,INSERT_VALUES));
    }
  }
  PetscCall(MatAssemblyBegin(cross_covariance,MAT_FINAL_ASSEMBLY));
  PetscCall(MatAssemblyEnd(cross_covariance,MAT_FINAL_ASSEMBLY));
  // Stop time measurement assembly
  PetscCall(PetscTime(&t_stop_assemble));
  //////////////////////////////////////////////////////////////////////////////
  // PART 2: CHOLESKY SOLVE
  // Start time measurement
  PetscCall(PetscTime(&t_start_solve));
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
  // Compute alpha
  PetscCall(KSPSolve(ksp,y_train,alpha));
  // Stop time measurement
  PetscCall(PetscTime(&t_stop_solve));
  //////////////////////////////////////////////////////////////////////////////
  // PART 3: PREDICTION
  // Start time measurement
  PetscCall(PetscTime(&t_start_predict));
  // Make predictions
  PetscCall(MatMult(cross_covariance,alpha,test_prediction));
  // Compute 2-norm between vectors
  PetscCall(VecAXPY(y_test,-1.0,test_prediction));
  PetscCall(VecNorm(y_test,NORM_2,&error));
  //////////////////////////////////////////////////////////////////////////////
  // Stop time measurement
  PetscCall(PetscTime(&t_stop_predict));
  //////////////////////////////////////////////////////////////////////////////
  // print output information
  PetscCall(PetscPrintf(PETSC_COMM_WORLD,"%d;%d;%d;%d;%lf;%lf;%lf;%lf;%lf;\n",
                        n_cores,
                        n_train,
                        n_test,
                        n_regressors,
                        t_stop_predict - t_start_assemble,
                        t_stop_assemble - t_start_assemble,
                        t_stop_solve - t_start_solve,
                        t_stop_predict - t_start_predict,
                        error / n_test));
  // Free work space
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
