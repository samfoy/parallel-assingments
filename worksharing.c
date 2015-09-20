#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNKSIZE 10
#define N 100

int main (int argc, char *argv[]) {
    int nthreads, tid, i, chunk;
    float a[N], b[N], c[N];
    double start, end; //for timing

    for (i=0; i < N; i++) {
        a[i] = b[i] = c[i] = i * 1.0;
    }

    chunk = CHUNKSIZE;
    start = omp_get_wtime();

    #pragma omp parallel shared(a,b,c,nthreads,chunk) private(i, tid)
    {
        tid = omp_get_thread_num();

        if (tid == 0) {
          nthreads = omp_get_num_threads();
          printf("Number of threads = %d\n", nthreads);
        }
        printf("Thread %d starting...\n", tid);

        #pragma omp for schedule(guided,chunk)
        for (i = 0; i < N; i++) {
          c[i] = a[i] + b[i];
          printf("Thread %d: c[%d]= %f\n", tid, i, c[i]);
        }
    } //exit parallel section

    end = omp_get_wtime();
    printf("Time of computation: %f seconds\n", end - start);
    return(0);
}
