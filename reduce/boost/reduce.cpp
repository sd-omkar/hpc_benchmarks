#include <time.h>
#include <sys/time.h>
#include <boost/compute.hpp>

//#define NUM_PLAT 0
//#define NUM_DEV 1
#define RUNS 100

namespace compute = boost::compute;

int main()
{
    // get the compute device
    std::vector<compute::platform> platforms = compute::system::platforms();
    std::vector<compute::device> devices = platforms[NUM_PLAT].devices();
    compute::device gpu = devices[NUM_DEV];
    std::cout << "Platform: " << platforms[NUM_PLAT].name() << "'" << std::endl;
    std::cout << "Device: " << gpu.name() << std::endl;

    // create a compute context and command queue
    compute::context ctx(gpu);
    compute::command_queue queue(ctx, gpu);

    // generate random numbers on the host
    srand(time(NULL));
    std::vector<float> host_vector(1000000);
    std::generate(host_vector.begin(), host_vector.end(), rand);
    //std::fill(host_vector.begin(), host_vector.end(), 2);

    // timing
    struct timeval start, end;
    float sum;
    
    // create vector on the device
    compute::vector<float> device_vector(1000000, ctx);

    // copy data to the device
    gettimeofday(&start, NULL);

    // reduce the data
    for (int i=0; i<RUNS; i++) {
    compute::copy(host_vector.begin(), host_vector.end(), device_vector.begin(), queue);
    compute::reduce(device_vector.begin(), device_vector.end(), &sum, compute::plus<float>(), queue);
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
    }

    // print time
    gettimeofday(&end, NULL);
    float time = (end.tv_usec + 1000000 * end.tv_sec) - (start.tv_usec + 1000000 * start.tv_sec);
    std::cout << "Time = " << time/RUNS/1000 << " ms"<< std::endl;
    //std::cout << "Sum = " << sum << std::endl;
    return 0;
}
