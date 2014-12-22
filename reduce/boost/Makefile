all: gpu cpu amd mic

gpu:
	nvcc -DNUM_PLAT=0 -DNUM_DEV=0 -I/usr/local/cuda/6.5.11/cuda/include/CL/ -I/usr/local/boost/1.53.0/include/ -L/usr/local/cuda/6.5.11/cuda/lib64/ -lOpenCL reduce.cpp -o gpu.out

intel:
	icpc -DNUM_PLAT=1 -DNUM_DEV=0 -I/opt/intel/opencl-sdk/include/CL/ -I/usr/local/boost/1.53.0/include/ -L/opt/intel/opencl/lib64 -lOpenCL reduce.cpp -o intel.out

amd:
	g++ -DNUM_PLAT=1 -DNUM_DEV=0 -I/opt/AMDAPPSDK-2.9-1/include/ -I/usr/local/boost/1.53.0/include/ -L/opt/AMDAPPSDK-2.9-1/lib/x86_64/ -lOpenCL reduce.cpp -o amd.out

mic:
	icpc -DNUM_PLAT=1 -DNUM_DEV=1 -I/opt/intel/opencl-sdk/include/CL/ -I/usr/local/boost/1.53.0/include/ -L/opt/intel/opencl/lib64 -lOpenCL reduce.cpp -o mic.out

list:
	g++ -I/opt/intel/opencl-sdk/include/CL/  -I/home/omkar/boost/include/ -L/opt/intel/opencl/lib64 -lOpenCL list_devices.cpp -o list.out
