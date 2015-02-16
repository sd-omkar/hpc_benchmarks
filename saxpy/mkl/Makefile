INC	:= /usr/local/sbel/intel/2013sp1/mkl/include
LIB	:= /usr/local/sbel/intel/2013sp1/mkl/lib/intel64
FLAGS	:= -O3 -openmp

all: mkl mic
	
mkl:
	icc -openmp -O3 -w -I/usr/local/sbel/intel/2013sp1/mkl/include saxpy.c common_func.c -L/usr/local/sbel/intel/2013sp1/mkl/lib/intel64 /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_intel_lp64.a -Wl,--start-group /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_intel_thread.a /usr/local/sbel/intel/2013sp1/mkl/lib/intel64/libmkl_core.a -Wl,--end-group -L/usr/local/sbel/intel/2013sp1/mkl/../composerxe/lib/intel64 -liomp5 -lpthread -ldl -lm -o mkl.out
	
mic:
	icc -mmic -openmp -O3 -w -I/usr/local/sbel/intel/2013sp1/mkl/include saxpy.c common_func.c -L/usr/local/sbel/intel/2013sp1/mkl/lib/mic /usr/local/sbel/intel/2013sp1/mkl/lib/mic/libmkl_intel_lp64.a -Wl,--start-group /usr/local/sbel/intel/2013sp1/mkl/lib/mic/libmkl_intel_thread.a /usr/local/sbel/intel/2013sp1/mkl/lib/mic/libmkl_core.a -Wl,--end-group -L/usr/local/sbel/intel/2013sp1/mkl/../composerxe/lib/mic -liomp5 -lpthread -ldl -lm -o mic.out
	
