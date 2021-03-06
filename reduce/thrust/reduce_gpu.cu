#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/copy.h>
#include <thrust/scan.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <time.h>

#define RUNS 25

using namespace std;

// Multiplier functor
struct multiplier{
  __host__ __device__
  float operator()(float x) {
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
  thrust::host_vector<float> h_data(size);
  thrust::host_vector<float> h_result(size);
  float h_sum, d_sum;

  // Timing varibles
  float time_reduce; 
  //double time_scan;
  cudaEvent_t start_reduce, end_reduce;
  //cudaEvent_t start_scan, end_scan;
  cudaEventCreate(&start_reduce);
  cudaEventCreate(&end_reduce);
  //cudaEventCreate(&start_scan);
  //cudaEventCreate(&end_scan);

  // Ready host and device data
  srand(time(NULL));
  thrust::generate(h_data.begin(), h_data.end(), rand);
  //thrust::fill(h_data.begin(), h_data.end(), 1);

  // Reduction
  thrust::device_vector<float> d_data = h_data;
  d_sum = thrust::transform_reduce(d_data.begin(), d_data.end(), multiplier(), (float)0, thrust::plus<float>());
  
  cudaEventRecord(start_reduce, NULL);
  
  for (int i=0; i<RUNS; i++) {
  d_sum = thrust::transform_reduce(d_data.begin(), d_data.end(), multiplier(), (float)0, thrust::plus<float>());
  }

  cudaEventRecord(end_reduce, NULL);
  cudaEventSynchronize(end_reduce);
  cudaEventElapsedTime(&time_reduce, start_reduce, end_reduce);
  
  //h_sum = thrust::transform_reduce(h_data.begin(), h_data.end(), multiplier(), (float)0, thrust::plus<float>());

  // Exclusive scan
  /*
  cudaEventRecord(start_scan, NULL);
  thrust::device_vector<float> d_result = h_data;
  thrust::exclusive_scan(d_result.begin(), d_result.end(), d_result.begin());

  thrust::copy(d_result.begin(), d_result.end(), h_result.begin());

  cudaEventRecord(end_scan, NULL);
  cudaEventSynchronize(end_scan);
  cudaEventElapsedTime(&time_scan, start_scan, end_scan);
  thrust::exclusive_scan(h_data.begin(), h_data.end(), h_data.begin());
  */

  
  cout << "name = reduction" << endl;
  cout << "input = vector" << endl;
  cout << "datatype = float" << endl;
  cout << "dim_x = " << size << endl;
  cout << "dim_y = 1" << endl;
  cout << "NNZ = " << size << endl;
  cout << "value_type = GFLOPS" << endl;
  //cout << "Reduction time: " << time_reduce/RUNS << " ms"<< endl;
  double time_sec = time_reduce / RUNS / 1e3;
  double gflops = 2 * size / time_sec / 1e9;
  cout << "value = " << gflops << endl;

  /*
  cout << "\tHost result: " << h_sum << endl;
  cout << "\tDevice result: " << d_sum << endl;
  double residue = (d_sum - h_sum) / h_sum;
  cout << "Residue: " << residue << endl;

  if (thrust::equal(h_data.begin(), h_data.end(), h_result.begin())) {
  cout << "Prefix scan time: " << time_scan << endl;
  cout << "\tHost result: " << h_data[size-1] << endl;
  cout << "\tDevice result: " << h_result[size-1] << endl;
  }
  else {
    printf("Mismatch in scan results\n");
    // Only for debugging
    for(std::vector<float>::size_type i = 0; i != h_data.size(); i++) 
    {
      cout << h_data[i] << endl;
      cout << h_result[i] << endl;
    }
  }
  */

  return 0;
}
