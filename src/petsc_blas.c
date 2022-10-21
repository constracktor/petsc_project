#include <stdio.h>
#include <petscksp.h>
#include <petsctime.h>
#define TYPE "%lf"

#undef __FUNCT__
#define __FUNCT__ "main"

int main(int argc,char **args)
{
  // Petsc structures
  PetscInt       n_loop = 500;
  PetscInt       exp_start = 1;
  PetscInt       exp_end = 3;
  PetscInt       early_stop = 15 * pow(10, exp_end - 1);
  PetscInt       n_vector[9 * (exp_end - exp_start) + 1];
  PetscInt       n_dim;
  PetscInt       n_cores,warmup;
  PetscInt       i,j,k,loop;
  PetscLogDouble t_start_potrf, t_stop_potrf, t_start_trsm, t_stop_trsm, t_start_gemm, t_stop_gemm;
  PetscLogDouble t_total_potrf = 0.0, t_total_trsm = 0.0, t_total_gemm = 0.0;
  Vec            ones;
  Mat            M, M_T, M_solved, M_solved_T, L, B, C;
  KSP            ksp;          // linear solver context
  PC             pc;           // preconditioner context
  // Petsc initialization
  PetscCall(PetscInitialize(&argc,&args,(char*)0,PETSC_NULL));
  PetscCallMPI(MPI_Comm_size(PETSC_COMM_WORLD,&n_cores));
  // fill n_vector
  for (i = exp_start; i < exp_end; i++)
  {
    for (j = 1; j < 10; j++)
    {
      n_vector[(i - exp_start) * 9 + j - 1] = j * pow(10, i);
    }
  }
  n_vector[9 * (exp_end - exp_start)] = pow(10, exp_end);
  // print header
  PetscCall(PetscPrintf(PETSC_COMM_WORLD,"N;POTRF;TRSM;GEMM;loop;%d;\n", n_loop));
  // warm up
  for (k = 0; k < 100000; k++)
  {
    warmup = warmup + 1;
  }
  // loop
  for (k = 0; k < 9 * (exp_end - exp_start) + 1; k++)
  {
    n_dim = n_vector[k];
    // early stopping
    if (early_stop < n_dim)
    {
      break;
    }
    t_total_potrf = 0;

    for (loop = 0; loop < n_loop; loop++)
    {
      // initialize vector
      PetscCall(VecCreate(PETSC_COMM_WORLD,&ones));
      PetscCall(VecSetSizes(ones,PETSC_DECIDE,n_dim));
      PetscCall(VecSetFromOptions(ones));
      // initalize matrices
      PetscCall(MatCreate(PETSC_COMM_WORLD,&M));
      PetscCall(MatSetType(M, MATMPIDENSE));
      PetscCall(MatSetSizes(M,PETSC_DECIDE,PETSC_DECIDE,n_dim,n_dim));
      PetscCall(MatSetFromOptions(M));
      PetscCall(MatSetUp(M));
      PetscCall(MatDuplicate(M,MAT_DO_NOT_COPY_VALUES,&B));
      PetscCall(MatDuplicate(M,MAT_DO_NOT_COPY_VALUES,&M_solved));
      PetscCall(MatDuplicate(M,MAT_DO_NOT_COPY_VALUES,&C));
      // setup one vector
      PetscCall(VecSet(ones, (PetscScalar) 1.0));
      // setup random matrix
      PetscCall(MatSetRandom(B, PETSC_NULL));
      PetscCall(MatScale(B, (PetscScalar) 1.0 / n_dim));
      PetscCall(MatAssemblyBegin(B,MAT_FINAL_ASSEMBLY));
      PetscCall(MatAssemblyEnd(B,MAT_FINAL_ASSEMBLY));
      // setup random but positive definite matrix
      PetscCall(MatSetRandom(M, PETSC_NULL));
      PetscCall(MatCreateTranspose(M, &M_T));
      PetscCall(MatAXPY(M, (PetscScalar) 1.0, M_T, SAME_NONZERO_PATTERN));
      PetscCall(MatScale(M, (PetscScalar) 0.5 / n_dim));
      PetscCall(MatDiagonalSet(M, ones, ADD_VALUES));
      PetscCall(MatAssemblyBegin(M,MAT_FINAL_ASSEMBLY));
      PetscCall(MatAssemblyEnd(M,MAT_FINAL_ASSEMBLY));

      //////////////////////////////////////////////////////////////////////////////
      // BENCHMARK
      // time cholesky solve
      // precomputing
      PetscCall(MatConvert(M,MATELEMENTAL,MAT_INPLACE_MATRIX ,&M)); // Convert matrix to solver format
      PetscCall(KSPCreate(PETSC_COMM_WORLD,&ksp));
      PetscCall(KSPSetOperators(ksp,M,M));
      PetscCall(KSPGetPC(ksp,&pc));
      PetscCall(KSPSetType(ksp,KSPPREONLY));
      PetscCall(PCSetType(pc,PCCHOLESKY));
      PetscCall(PCFactorSetMatSolverType(pc,MATSOLVERELEMENTAL));
      //timing
      PetscCall(PetscTime(&t_start_potrf));
      PetscCall(KSPSetUp(ksp)); // KSPSolve not used as triangular solve not timed
      PetscCall(PetscTime(&t_stop_potrf));
      ////
      // time triangular solve
      // precomputing
      PetscCall(PCFactorGetMatrix(pc, &L));
      // timimg
      PetscCall(PetscTime(&t_start_trsm));
      PetscCall(MatMatSolve(L, B, M_solved));
      PetscCall(PetscTime(&t_stop_trsm));
      ////
      // time matrix multiplication
      // precomputing
      PetscCall(MatCreateTranspose(M_solved, &M_solved_T));
      // timimg
      PetscCall(PetscTime(&t_start_gemm));
      PetscCall(MatMatMult(M_solved, M_solved_T, MAT_REUSE_MATRIX, PETSC_DEFAULT, &C)); //  C =  M_solved * M_solved^T
      PetscCall(MatAXPY(B, (PetscScalar) -1.0, C, SAME_NONZERO_PATTERN)); // B = B - C
      PetscCall(PetscTime(&t_stop_gemm));
      //////////////////////////////////////////////////////////////////////////////
      // add time difference to total time
      t_total_potrf += t_stop_potrf - t_start_potrf;
      t_total_trsm += t_stop_trsm - t_start_trsm;
      t_total_gemm += t_stop_gemm - t_start_gemm;
      // destroy Petsc structures
      PetscCall(MatDestroy(&M));
      PetscCall(MatDestroy(&M_T));
      PetscCall(MatDestroy(&M_solved));
      PetscCall(MatDestroy(&M_solved_T));
      PetscCall(MatDestroy(&B));
      PetscCall(MatDestroy(&C));
      PetscCall(KSPDestroy(&ksp));
    }
    // print output information
    PetscCall(PetscPrintf(PETSC_COMM_WORLD,"%d;%lf;%lf;%lf;\n", n_dim, t_total_potrf / n_loop, t_total_trsm / n_loop, t_total_gemm / n_loop));
  }
  // finalize Petsc
  PetscCall(PetscFinalize());
}
