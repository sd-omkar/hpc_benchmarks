#include <stdio.h>
#include <stdlib.h>

#define RUNS 50

int main(int argc, char *argv[])
{
      int       n, incx=1, incy=1;
      float     alpha = 5.f;
      float    *x, *y;

      // Get input parameters
      if( argc != 2 ) {
         printf("Usage: %s N\n", argv[0]);
         exit(1);
      }
      
      n = (10 + atoi(argv[1])) * 1024 * 1024;

      x    = (float *)calloc( n, sizeof( float ) );
      y    = (float *)calloc( n, sizeof( float ) );
      
      if( x == NULL || y == NULL ) {
          printf( "\n Can't allocate memory for arrays\n");
          return 1;
      }

      // Generate x and y
      srand(time(NULL));
      int i;
      for (i=0; i<n; i++) {
          x[i] = (float)rand()/(float)RAND_MAX;
          y[i] = (float)rand()/(float)RAND_MAX;
      }

      // Timing variables
      double time_saxpy;
      struct timeval start, end;

      // Call CBLAS_SAXPY subroutine
      cblas_saxpy(n, alpha, x, incx, y, incy);

      gettimeofday(&start, NULL);

      for (i=0; i<RUNS; i++) {
          cblas_saxpy(n, alpha, x, incx, y, incy);
      }

      gettimeofday(&end, NULL);
      time_saxpy = (end.tv_usec + 1e6 * end.tv_sec)
                    - (start.tv_usec + 1e6 * start.tv_sec);

      // Print timing results
      printf("SAXPY time: %lf ms\n", time_saxpy/1e3/RUNS);
      double time_sec = time_saxpy / RUNS / 1e6;
      double gflops = 2 * n / time_sec / 1e9;
      printf("N: %d\t GFLOPS: %lf\n", n, gflops);

      free(x);
      free(y);

      return 0;
}
