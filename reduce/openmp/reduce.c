#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define SIZE 1000000
#define RUNS 1000

main ()  {

int i;
int n = SIZE;
float result;
float *array = (float *)malloc(sizeof(float)*SIZE);

/* Some initializations */
result = 0.0;
srand(time(NULL));
omp_set_num_threads(236);

#pragma omp parallel for
for (i=0; i < n; i++)
  array[i] = rand();

double start = omp_get_wtime();
for (j=0; j<RUNS; j++) {
  #pragma omp parallel for reduction(+:result)  
    for (i=0; i < n; i++)
      result = result + array[i];
}
double end= omp_get_wtime();

printf("Time = %5.9lf s\n", (end - start)/RUNS);
}
