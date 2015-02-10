INC	:= /usr/local/sbel/intel/2013sp1/mkl/include
LIB	:= /usr/local/sbel/intel/2013sp1/mkl/lib/intel64
FLAGS	:= -O3 -openmp

all: mkl mic
	
mkl:
	icc -g -openmp -O3 -w -I/usr/local/sbel/intel/2013sp1/mkl/include cblas_saxpyx.c -L/usr/local/sbel/intel/2013sp1/mkl/lib/intel64 /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_intel_lp64.so -Wl,--start-group /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_intel_thread.so /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_core.so -Wl,--end-group -L/usr/local/sbel/intel/2013sp1/mkl/../compiler/lib/intel64 -liomp5 -lpthread -ldl -lm -o mkl.out

mic:
	icc -mmic -O3 -openmp -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP -I/usr/local/cuda/6.5.11/cuda/include/ reduce_omp.c -o mic.out
