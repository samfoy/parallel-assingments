#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 256
#define THREADS 16

bool errorCheck(int n, double **A, double **B) {
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      if ((A[i][j] - B[i][j] > 0.001) || (B[i][j] - A[i][j] > 0.001))
        return true;
  return false;
}

int sequential(int n, double **A, double **B, double **R) {
  int i, j, k;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      R[i][j] = 0;
      for (k = 0; k < n; k++)
        R[i][j] += A[i][k]*B[k][j];
      }
  return 0;
}

int parallelOuter(int n, double **A, double **B, double **R) {
  int i, j, k;

  #pragma omp parallel shared(A,B,R,n) private(i,j,k)
  {
    #pragma omp for
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++) {
        R[i][j] = 0;
        for (k = 0; k < n; k++)
          R[i][j] += A[i][k]*B[k][j];
        }
  }
  return 0;
}

int parallelMiddle(int n, double **A, double **B, double **R) {
  int i, j, k;

  #pragma omp parallel shared(A,B,R,n) private(i,j,k)
  {
    for (i = 0; i < n; i++)
      #pragma omp for
      for (j = 0; j < n; j++) {
        R[i][j] = 0;
        for (k = 0; k < n; k++)
          R[i][j] += A[i][k]*B[k][j];
      }
  }
  return 0;
}

int parallelBoth(int n, double **A, double **B, double **R) {
  int i, j, k;

  #pragma omp parallel shared(A,B,R,n) private(i,j,k)
  {
    #pragma omp for collapse(2)
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++) {
        R[i][j] = 0;
        for (k = 0; k < n; k++)
          R[i][j] += A[i][k]*B[k][j];
      }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  omp_set_num_threads(THREADS);

  int i, j;
  double start, end;
  double **A, **B, **C, **D;

  A = B = C = D = (double **)malloc(N * sizeof(double *));
  for (i = 0; i < N; i++) {
    A[i] = B[i] = C[i] = D[i] = (double *)malloc(N * sizeof(double));
  }

  // set initial values
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i][j] = j*1;
      B[i][j] = i*j+2;
    }
  }

  // loop sequential
  start = omp_get_wtime();
  for (i = 0; i < 10; i++)
    sequential(N, A, B, C);
  end = omp_get_wtime();
  printf("Average time of sequential computation: %f seconds\n", (end - start)/10.0);

  // loop parallel outer
  start = omp_get_wtime();
  for (i = 0; i < 10; i++)
    parallelOuter(N, A, B, D);
  end = omp_get_wtime();
  printf("Average time of parallel outer computation: %f seconds\n", (end - start)/10.0);

  if (errorCheck(N, C, D)) {
    printf("ERROR, sequential and parallel outer versions give different answers");
    return 1;
  }

  // loop parallel middle
  start = omp_get_wtime();
  for (i = 0; i < 10; i++)
    parallelMiddle(N, A, B, D);
  end = omp_get_wtime();
  printf("Average time of parallel middle computation: %f seconds\n", (end - start)/10.0);

  if (errorCheck(N, C, D)) {
    printf("ERROR, sequential and parallel middle versions give different answers");
    return 1;
  }

  // loop parallel both
  start = omp_get_wtime();
  for (i = 0; i < 10; i++)
    parallelBoth(N, A, B, D);
  end = omp_get_wtime();
  printf("Average time of parallel both computation: %f seconds\n", (end - start)/10.0);

  if (errorCheck(N, C, D)) {
    printf("ERROR, sequential and parallel both versions give different answers");
    return 1;
  }

  return 0;
}
