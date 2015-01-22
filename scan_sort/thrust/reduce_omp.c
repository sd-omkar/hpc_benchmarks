#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/copy.h>
#include <thrust/scan.h>
#include <thrust/sort.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <numeric>
#include <time.h>
#include <sys/time.h>
#include <random>

#define RUNS 20

using namespace std;

// Multiplier functor
struct multiplier{
  __host__ __device__
  double operator()(double x) {
    return (x * x);
  }
};

int main (int argc, char *argv[]) {
  // Basic error check
  if (argc != 2) {
    printf("Usage: %s N", argv[0]);
    exit(1);
  }

  // Select device
  //cudaSetDevice(0);

  // Get size
  int size = 1024 * 1024 * (10 + atoi(argv[1]));

  // Initialize vectors
  thrust::host_vector<double> h_data(size);
  thrust::host_vector<long> h_scan(size);
  thrust::host_vector<int> h_sort(size);
  //double h_sum, d_sum;

  // Timing varibles
  //float time_reduce;
  //cudaEvent_t start_reduce, end_reduce;
  double time_sort, time_scan;
  struct timeval start_scan, end_scan;
  struct timeval start_sort, end_sort;

  // Ready host and device data
  srand(time(NULL));
  thrust::generate(h_data.begin(), h_data.end(), rand);
  //thrust::fill(h_data.begin(), h_data.end(), 1);

  // Exclusive scan
  std::default_random_engine rng( std::rand() );
  std::uniform_int_distribution<long> rnd_long;

  thrust::generate(h_scan.begin(), h_scan.end(), [&]() { return rnd_long(rng); });
  thrust::host_vector<long> d_scan = h_scan;
  thrust::exclusive_scan(d_scan.begin(), d_scan.end(), d_scan.begin());
  
  gettimeofday(&start_scan, NULL);

  for (int i=0; i<RUNS; i++)
      thrust::exclusive_scan(d_scan.begin(), d_scan.end(), d_scan.begin());

  gettimeofday(&end_scan, NULL);

  time_scan = (end_scan.tv_usec + 1000000 * end_scan.tv_sec)
                - (start_scan.tv_usec + 1000000 * start_scan.tv_sec);
  time_scan /= 1000;
  
  cout << "Scan time: " << time_scan/RUNS << " ms"<< endl;
  double time_sec = time_scan / RUNS / 1e3;
  cout << "Scan N:" << size << "\tkeys/sec: " << size / time_sec << endl;

  // Sort
  std::uniform_int_distribution<long> rnd_int;

  thrust::generate(h_sort.begin(), h_sort.end(), [&]() { return rnd_int(rng); });
  thrust::host_vector<int> d_sort = h_sort;
  thrust::sort(d_sort.begin(), d_sort.end());
  
  gettimeofday(&start_sort, NULL);

  for (int i=0; i<RUNS; i++)
      thrust::sort(d_sort.begin(), d_sort.end());

  gettimeofday(&end_sort, NULL);

  time_sort = (end_sort.tv_usec + 1000000 * end_sort.tv_sec)
                - (start_sort.tv_usec + 1000000 * start_sort.tv_sec);
  time_sort /= 1000;

  cout << "Sort time: " << time_sort/RUNS << " ms"<< endl;
  time_sec = time_sort / RUNS / 1e3;
  cout << "Sort N:" << size << "\tkeys/sec: " << size / time_sec << endl;

  return 0;
}
