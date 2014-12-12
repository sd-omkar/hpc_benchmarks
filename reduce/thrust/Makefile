all: gpu omp tbb
	
gpu:
	nvcc -O3 reduce_gpu.cu -o gpu.out
	
omp:
	g++ -O3 -fopenmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -lgomp -I/usr/local/cuda/6.5.11/cuda/include/ reduce_omp.c -o omp.out
	
tbb:
	icpc -O3 -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_TBB -I/usr/local/cuda/6.5.11/cuda/include/ -L/usr/local/sbel/intel/2013sp1/tbb/lib/intel64/gcc4.4/ -ltbb reduce_omp.c -o tbb.out
	
mic:
	icpc -mmic -O3 -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_TBB -I/usr/local/cuda/6.5.11/cuda/include/ -L/usr/local/sbel/intel/2013sp1/tbb/lib/mic/ -ltbb -lpthread reduce_omp.c -o mic.out
