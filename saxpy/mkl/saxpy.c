/*******************************************************************************
!   Copyright(C) 1999-2013 Intel Corporation. All Rights Reserved.
!   
!   The source code, information  and  material ("Material") contained herein is
!   owned  by Intel Corporation or its suppliers or licensors, and title to such
!   Material remains  with Intel Corporation  or its suppliers or licensors. The
!   Material  contains proprietary information  of  Intel or  its  suppliers and
!   licensors. The  Material is protected by worldwide copyright laws and treaty
!   provisions. No  part  of  the  Material  may  be  used,  copied, reproduced,
!   modified, published, uploaded, posted, transmitted, distributed or disclosed
!   in any way  without Intel's  prior  express written  permission. No  license
!   under  any patent, copyright  or  other intellectual property rights  in the
!   Material  is  granted  to  or  conferred  upon  you,  either  expressly,  by
!   implication, inducement,  estoppel or  otherwise.  Any  license  under  such
!   intellectual  property  rights must  be express  and  approved  by  Intel in
!   writing.
!   
!   *Third Party trademarks are the property of their respective owners.
!   
!   Unless otherwise  agreed  by Intel  in writing, you may not remove  or alter
!   this  notice or  any other notice embedded  in Materials by Intel or Intel's
!   suppliers or licensors in any way.
!
!*******************************************************************************
!  Content:
!      C B L A S _ S A X P Y   Example Program Text ( C Interface )
!******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "mkl_example.h"
#define RUNS 100

int main(int argc, char *argv[])
{
      FILE *in_file;
      char *in_file_name;

      MKL_INT   n, incx, incy;
      float     alpha;
      float    *x, *y, *c;
      MKL_INT   len_x, len_y;

/*       Get input parameters                                  */

      if( argc != 2 ) {
         printf("Usage: %s N", argv[0]);
         return 1;
      }

/*       Get input data                                        */

      n = (10 + atoi(argv[1])) * 1024 * 1024;
      //n = 10;
      incx = incy = 1;
      alpha = 1.f;

      len_x = 1+(n-1)*abs(incx);
      len_y = 1+(n-1)*abs(incy);
      x    = (float *)calloc( len_x, sizeof( float ) );
      y    = (float *)calloc( len_y, sizeof( float ) );
      c    = (float *)calloc( len_y, sizeof( float ) );

      if( x == NULL || y == NULL || c == NULL) {
          printf( "\n Can't allocate memory for arrays\n");
          return 1;
      }

      // Generate x and y
      srand(time(NULL));
      int i;
      for (i=0; i<n; i++) {
          x[i] = (float)rand()/(float)RAND_MAX;
          y[i] = (float)rand()/(float)RAND_MAX;
          //x[i] = y[i] = 1.f;
      }

      // Timing variables
      double time_saxpy;
      struct timeval start, end;

      /* DEBUG
      printf("N = %d\t alpha = %f\n", n, alpha);

      for (i=0; i<n; i++)
          c[i] = alpha * x[i] + y[i];
      cblas_saxpy(n, alpha, x, incx, y, incy);

      printf("\n\n     OUTPUT DATA");
      PrintVectorS(FULLPRINT, n, y, incy, "Y");
      PrintVectorS(FULLPRINT, n, c, incy, "C");
      */

      gettimeofday(&start, NULL);

      for (i=0; i<RUNS; i++) {
          cblas_saxpy(n, alpha, x, incx, y, incy);
      }

      gettimeofday(&end, NULL);
      time_saxpy = (end.tv_usec + 1e6 * end.tv_sec)
                    - (start.tv_usec + 1e6 * start.tv_sec);

      // Print timing results
      //printf("SAXPY time: %lf ms\n", time_saxpy/1e3/RUNS);
      double time_sec = time_saxpy / RUNS / 1e6;
      double gflops = 2 * n / time_sec / 1e9;
      //printf("N: %d\t GFLOPS: %lf\n", n, gflops);
      printf("name = saxpy\n");
      printf("input = vector\n");
      printf("datatype = float\n");
      printf("dim_x = %ld\n", n);
      printf("dim_y = %ld\n", 1);
      printf("NNZ = %ld\n", n);
      printf("value_type = GFLOPS\n");
      printf("value = %lf\n", gflops);
      
      free(x);
      free(y);
      free(c);

      return 0;
}

