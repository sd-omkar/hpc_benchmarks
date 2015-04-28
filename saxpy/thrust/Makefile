all: gpu omp mic
	
gpu:
	nvcc -O3 -arch=sm_35 saxpy_gpu.cu -o gpu.out
	
omp:
	g++ -O3 -mavx -fopenmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -lgomp -I/usr/local/cuda/6.5.11/cuda/include/ saxpy_omp.c -o omp.out
	icpc -O3 -mavx -openmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -I/usr/local/cuda/6.5.11/cuda/include/ -I/usr/local/include saxpy_omp.c -o icc.out
	
mic:
	icpc -mmic -O3 -openmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -I/usr/local/cuda/6.5.11/cuda/include/ saxpy_omp.c -o mic.out
