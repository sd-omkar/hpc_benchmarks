#include <thrust/transform.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/functional.h>
#include <thrust/generate.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <sys/time.h>

#define A 5.f
#define RUNS 25

using namespace std;

struct saxpy_functor: public thrust::binary_function<float,float,float> {
    const float a;
    saxpy_functor(float _a) : a(_a) {}
    __host__ __device__
    float operator()(const float &x, const float &y) const { 
        return a * x + y;
    }
};

int main (int argc, char *argv[]) {
    // Check argument
    if (argc != 2) {
        printf("Usage: %s N", argv[0]);
        exit(1);
    }

    // Get size
    int size = 1024 * 1024 * (10 + atoi(argv[1]));

    // Initialize host vectors
    thrust::host_vector<float> h_X(size);
    thrust::host_vector<float> h_Y(size);

    // Timing variables
    double time_saxpy;
    struct timeval start, end;

    // Generate vectors
    srand(time(NULL));
    thrust::generate(h_X.begin(), h_X.end(), rand);
    thrust::generate(h_Y.begin(), h_Y.end(), rand);

    // SAXPY
    // Y <- A*X + Y 
    thrust::transform(h_X.begin(), h_X.end(),
                      h_Y.begin(), h_Y.begin(),
                      saxpy_functor(A));

    gettimeofday(&start, NULL);

    for (int i=0; i<RUNS; i++)
        thrust::transform(h_X.begin(), h_X.end(),
                          h_Y.begin(), h_Y.begin(),
                          saxpy_functor(A));

    gettimeofday(&end, NULL);
    time_saxpy = (end.tv_usec + 1e6 * end.tv_sec)
                  - (start.tv_usec + 1e6 * start.tv_sec);

    cout << "SAXPY time: " << time_saxpy / 1e3 / RUNS << " ms" << endl;
    double time_sec = time_saxpy / RUNS / 1e6;
    double gflops = 2 * size / time_sec / 1e9;
    cout << "N: " << size << "\tGFLOPS: " << gflops << endl;

    return 0;
}
