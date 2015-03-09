#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <blaze/Math.h>
#include <sys/time.h>

#define RUNS 1

using blaze::DynamicVector;
using namespace std;

int main (int argc, char *argv[]) {
    // Check argument
    if (argc != 2) {
        printf("Usage: %s N", argv[0]);
        exit(1);
    }

    // Get size
    int size = 1024 * 1024 * (atoi(argv[1]));

    // Initialize vectors
    DynamicVector<float> a(size), b(size), c(size);

    // Timing variables
    double time_saxpy;
    struct timeval start, end;

    // Generate vectors
    srand(time(NULL));
    for (int i=0; i<size; i++) {
        a[i] = (float)rand()/(float)RAND_MAX;
        b[i] = (float)rand()/(float)RAND_MAX;
        c[i] = 0.f;
    }

    // SAXPY
    c = a * 5.f + b;

    gettimeofday(&start, NULL);

    for (int i=0; i<RUNS; i++)
        c = a * 5.f + b;

    gettimeofday(&end, NULL);
    time_saxpy = (end.tv_usec + 1e6 * end.tv_sec)
                  - (start.tv_usec + 1e6 * start.tv_sec);

    cout << "===========" << endl;
    cout << "SAXPY time: " << time_saxpy / 1e3 / RUNS << " ms" << endl;
    double time_sec = time_saxpy / RUNS / 1e6;
    double gflops = 2 * size / time_sec / 1e9;
    cout << "N: " << size << "\nGFLOPS: " << gflops << endl;

    return 0;
}
