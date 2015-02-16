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
#include <time.h>

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

    // Select device
    cudaSetDevice(0);

    // Get size
    int size = 1024 * 1024 * (10 + atoi(argv[1]));

    // Initialize host vectors
    thrust::host_vector<float> h_X(size);
    thrust::host_vector<float> h_Y(size);

    // Timing variables
    float time_saxpy;
    cudaEvent_t start_saxpy, end_saxpy;
    cudaEventCreate(&start_saxpy);
    cudaEventCreate(&end_saxpy);

    // Generate vectors
    srand(time(NULL));
    thrust::generate(h_X.begin(), h_X.end(), rand);
    thrust::generate(h_Y.begin(), h_Y.end(), rand);

    // SAXPY
    // Y <- A*X + Y 
    thrust::device_vector<float> d_X = h_X;
    thrust::device_vector<float> d_Y = h_Y;
    thrust::transform(d_X.begin(), d_X.end(),
                      d_Y.begin(), d_Y.begin(),
                      saxpy_functor(A));

    cudaEventRecord(start_saxpy, NULL);

    for (int i=0; i<RUNS; i++)
        thrust::transform(d_X.begin(), d_X.end(),
                          d_Y.begin(), d_Y.begin(),
                          saxpy_functor(A));

    cudaEventRecord(end_saxpy, NULL);
    cudaEventSynchronize(end_saxpy);
    cudaEventElapsedTime(&time_saxpy, start_saxpy, end_saxpy);

    cout << "SAXPY time: " << time_saxpy / RUNS << " ms" << endl;
    double time_sec = time_saxpy / RUNS / 1e3;
    double gflops = 2 * size / time_sec / 1e9;
    cout << "N: " << size << "\tGFLOPS: " << gflops << endl;

    return 0;
}
