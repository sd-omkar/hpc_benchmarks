all: gpu
	
gpu:
	nvcc -O3 -arch=sm_35 saxpy_gpu.cu -o gpu.out
	
omp:
	g++ -O3 -fopenmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -lgomp -I/usr/local/cuda/6.5.11/cuda/include/ saxpy_omp.c -o omp.out
	
tbb:
	icpc -O3 -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_TBB -I/usr/local/cuda/6.5.11/cuda/include/ -L/usr/local/sbel/intel/2013sp1/tbb/lib/intel64/gcc4.4/ -ltbb saxpy_omp.c -o tbb.out
	
mic:
	icpc -mmic -O3 -openmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -I/usr/local/cuda/6.5.11/cuda/include/ saxpy_omp.c -o mic.out
