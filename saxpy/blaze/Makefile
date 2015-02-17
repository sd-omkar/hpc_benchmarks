BOOST	:= /usr/local/boost/1.53.0
all: omp cpp bst mic
	
omp:
	g++ -O3 -mavx -fopenmp -lgomp saxpy_omp.c -o omp.out
	
cpp:
	g++ -O3 -mavx -std=c++11 -pthread -DBLAZE_USE_CPP_THREADS saxpy_omp.c -o cpp.out
	
bst:
	g++ -O3 -mavx -DBLAZE_USE_BOOST_THREADS -I$(BOOST)/include/ saxpy_omp.c -o bst.out -L$(BOOST)/lib/ -lboost_thread
	
mic:
	icpc -mmic -O3 -openmp -I/usr/local/include -I$(BOOST)/include/ saxpy_omp.c -o mic.out

clean:
	rm -v *.out
