all:
	nvcc -O3 q1-2.cu -o gpu.out
	nvcc -O3 -Xcompiler -fopenmp -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP -lgomp -Xcompiler -O3 q1-2.cu -o omp.out
	nvcc -O3 -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_TBB -L/usr/local/sbel/intel/2013sp1/tbb/lib/intel64/gcc4.4/ -ltbb -Xcompiler -O3 q1-2.cu -o tbb.out
