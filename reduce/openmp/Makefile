all: omp mic

omp:
	g++ -O3 -fopenmp reduce.c -o omp.out

mic:
	icpc -O3 -mmic -openmp reduce.c -o mic.out
