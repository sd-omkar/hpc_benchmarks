CC		:= g++
CFLAGS		:= -O3 -std=gnu++11 -DHAVE_BOOST_COMPUTE=1
INC_BOOST	:= /usr/local/boost/1.53.0/include/
INC_CL		:= /opt/intel/opencl-sdk/include/CL/
LIB_CL		:= /opt/intel/opencl/lib64/
LIB_BOOST	:= /usr/local/boost/1.53.0/lib/ 

all: ocl

ocl:
	$(CC) $(CFLAGS) -I$(INC_CL) -I$(INC_BOOST) -o ocl.out vector.cpp -L$(LIB_CL) -lOpenCL -L$(LIB_BOOST) -lboost_program_options -lboost_system -lboost_thread

bench:
	$(CC) $(CFLAGS) -I$(INC_CL) -I$(INC_BOOST) -o bench.out benchmark.cpp -L$(LIB_CL) -lOpenCL -L$(LIB_BOOST) -lboost_program_options -lboost_system -lboost_thread

clean:
	rm -fv *.out
