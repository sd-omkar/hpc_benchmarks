#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define SIZE 1000000

main ()  {

int i;
int n = SIZE;
float result;
float *array = (float *)malloc(sizeof(float)*SIZE);
struct timeval start, end;

/* Some initializations */
result = 0.0;
srand(time(NULL));
omp_set_num_threads(236);

#pragma omp parallel for
for (i=0; i < n; i++)
  array[i] = rand();

gettimeofday(&start, NULL);
#pragma omp parallel for reduction(+:result)  
  for (i=0; i < n; i++)
    result = result + array[i];
gettimeofday(&end, NULL);

float time = (end.tv_usec + 1000000 * end.tv_sec) - (start.tv_usec + 1000000 * start.tv_sec);
printf("Time = %f ms\n", time/1000);

}
