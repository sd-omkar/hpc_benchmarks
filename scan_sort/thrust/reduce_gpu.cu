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
#include <time.h>
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
  cudaSetDevice(1);

  // Get size
  int size = 1024 * 1024 * (10 + atoi(argv[1]));

  // Initialize vectors
  thrust::host_vector<double> h_data(size);
  thrust::host_vector<int> h_scan(size);
  thrust::host_vector<long> h_sort(size);
  //double h_sum, d_sum;

  // Timing varibles
  //float time_reduce;
  //cudaEvent_t start_reduce, end_reduce;
  float time_sort, time_scan;
  cudaEvent_t start_scan, end_scan;
  cudaEvent_t start_sort, end_sort;
  //cudaEvent_t start_scan, end_scan;
  //cudaEventCreate(&start_reduce);
  //cudaEventCreate(&end_reduce);
  cudaEventCreate(&start_scan);
  cudaEventCreate(&end_scan);
  cudaEventCreate(&start_sort);
  cudaEventCreate(&end_sort);

  // Ready host and device data
  srand(time(NULL));
  thrust::generate(h_data.begin(), h_data.end(), rand);
  //thrust::fill(h_data.begin(), h_data.end(), 1);

  // Reduction
  /*
  thrust::device_vector<double> d_data = h_data;
  d_sum = thrust::transform_reduce(d_data.begin(), d_data.end(), multiplier(), (double)0, thrust::plus<double>());
  
  cudaEventRecord(start_reduce, NULL);
  
  for (int i=0; i<RUNS; i++) {
  d_sum = thrust::transform_reduce(d_data.begin(), d_data.end(), multiplier(), (double)0, thrust::plus<double>());
  }

  cudaEventRecord(end_reduce, NULL);
  cudaEventSynchronize(end_reduce);
  cudaEventElapsedTime(&time_reduce, start_reduce, end_reduce);
  h_sum = thrust::transform_reduce(h_data.begin(), h_data.end(), multiplier(), (double)0, thrust::plus<double>());
  
  cout << "Reduction time: " << time_reduce/RUNS << " ms"<< endl;
  double time_sec = time_reduce / RUNS / 1e3;
  double gflops = 2 * size / time_sec / 1e9;
  cout << "N:" << size << "\tGFLOPS: " << gflops << endl;

  //cout << "\tHost result: " << h_sum << endl;
  //cout << "\tDevice result: " << d_sum << endl;
  double residue = (d_sum - h_sum) / h_sum;
  cout << "Residue: " << residue << endl;
  */


  // Exclusive scan
  std::default_random_engine rng( std::rand() );
  std::uniform_int_distribution<int> rnd_int;

  thrust::generate(h_scan.begin(), h_scan.end(), [&]() { return rnd_int(rng); });
  thrust::device_vector<int> d_scan = h_scan;
  thrust::exclusive_scan(d_scan.begin(), d_scan.end(), d_scan.begin());
  
  cudaEventRecord(start_scan, NULL);

  for (int i=0; i<RUNS; i++)
      thrust::exclusive_scan(d_scan.begin(), d_scan.end(), d_scan.begin());

  cudaEventRecord(end_scan, NULL);
  cudaEventSynchronize(end_scan);
  cudaEventElapsedTime(&time_scan, start_scan, end_scan);

  //thrust::copy(d_scan.begin(), d_scan.end(), h_scan.begin());
  //thrust::exclusive_scan(h_data.begin(), h_data.end(), h_data.begin());
  
  cout << "Scan time: " << time_scan/RUNS << " ms"<< endl;
  double time_sec = time_scan / RUNS / 1e3;
  cout << "Scan N:" << size << "\tkeys/sec: " << size / time_sec << endl;

  /*
  if (thrust::equal(h_data.begin(), h_data.end(), h_scan.begin())) {
  cout << "Prefix scan time: " << time_scan << endl;
  cout << "\tHost result: " << h_data[size-1] << endl;
  cout << "\tDevice result: " << h_result[size-1] << endl;
  }
  else {
    printf("Mismatch in scan results\n");
    // Only for debugging
    for(std::vector<double>::size_type i = 0; i != h_data.size(); i++) 
    {
      cout << h_data[i] << endl;
      cout << h_result[i] << endl;
    }
  }
  */

  // Sort
  std::uniform_int_distribution<long> rnd_long;

  thrust::generate(h_sort.begin(), h_sort.end(), [&]() { return rnd_long(rng); });
  thrust::device_vector<long> d_sort = h_sort;
  thrust::sort(d_sort.begin(), d_sort.end());
  
  cudaEventRecord(start_sort, NULL);

  for (int i=0; i<RUNS; i++)
      thrust::sort(d_sort.begin(), d_sort.end());

  cudaEventRecord(end_sort, NULL);
  cudaEventSynchronize(end_sort);
  cudaEventElapsedTime(&time_sort, start_sort, end_sort);

  cout << "Sort time: " << time_sort/RUNS << " ms"<< endl;
  time_sec = time_sort / RUNS / 1e3;
  cout << "Sort N:" << size << "\tkeys/sec: " << size / time_sec << endl;

  return 0;
}
