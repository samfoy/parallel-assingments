#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  int nthreads, tid;

  omp_set_num_threads(32); // One method of declaring the number of threads
  #pragma omp parallel private(nthreads, tid) // Begin parallel stage
  {
    tid = omp_get_thread_num();
    printf("Hello World from thread = %d\n", tid);

    if (tid == 0) { // Only master thread executes
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d", nthreads);
    }
  } // Exit parallel stage
  return (0);
}
