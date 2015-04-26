#include <time.h>
#include <sys/time.h>
#include <boost/compute.hpp>

//#define NUM_PLAT 0
//#define NUM_DEV 1
#define RUNS 25

namespace compute = boost::compute;

int main(int argc, char *argv[])
{
    if (argc !=2 ) {
        printf("Usage: %s N\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    long size = 1024 * 1024 * n;

    // get the compute device
    std::vector<compute::platform> platforms = compute::system::platforms();
    std::vector<compute::device> devices = platforms[NUM_PLAT].devices();
    compute::device gpu = devices[NUM_DEV];
    //std::cout << "Platform: " << platforms[NUM_PLAT].name() << "'" << std::endl;
    //std::cout << "Device: " << gpu.name() << std::endl;

    // create a compute context and command queue
    compute::context ctx(gpu);
    compute::command_queue queue(ctx, gpu, compute::command_queue::enable_profiling);

    // generate random numbers on the host
    srand(time(NULL));
    std::vector<float> host_vector(size);
    std::generate(host_vector.begin(), host_vector.end(), rand);
    //std::fill(host_vector.begin(), host_vector.end(), 2);

    // timing
    struct timeval start, end;
    compute::future<void> copy;
    float sum;
    
    // create vector on the device
    compute::vector<float> device_vector(size, ctx);

    // copy data to the device
    copy = compute::copy_async(host_vector.begin(), host_vector.end(), device_vector.begin(), queue);
    copy.wait();

    gettimeofday(&start, NULL);

    // reduce the data
    for (int i=0; i<RUNS; i++) {
    compute::reduce(device_vector.begin(), device_vector.end(), &sum, compute::plus<float>(), queue);
    }

    /*
    // sort data on the device
    compute::sort(
        device_vector.begin(), device_vector.end(), queue
    );
    // copy data back to the host
    compute::copy(
        device_vector.begin(), device_vector.end(), host_vector.begin(), queue
    );
    */
    gettimeofday(&end, NULL);

    // Print metrics
    std::cout << "name = " << "reduction" << std::endl;
    std::cout << "input = " << "vector" << std::endl;
    std::cout << "datatype = " << "float" << std::endl;
    std::cout << "dim_x = " << size << std::endl;
    std::cout << "dim_y = " << 1 << std::endl;
    std::cout << "NNZ = " << size << std::endl;
    
    // Get time
    float time = (end.tv_usec + 1000000 * end.tv_sec) - (start.tv_usec + 1000000 * start.tv_sec);
    time /= 1e6;
    float gflops = size * RUNS / time / 1e9;
    std::cout << "value_type = " << "GFLOPS" << std::endl;
    std::cout << "value = " << gflops << std::endl;
    //std::cout << "Sum = " << sum << std::endl;
    return 0;
}
