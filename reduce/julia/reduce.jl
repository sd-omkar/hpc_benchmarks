# This is a nearly direct port of github.com/uwsbel/benchmarks to Julia

if !isinteractive()
    if size(ARGS)[1] < 1
        println("Usage: julia scriptname N [r]")
        println("       N: number of elements")
        println("       r: number of runs")
        exit(0)
    end

    N = integer(ARGS[1]);
    N = 1024*1024*(10 + integer(ARGS[1]));
    nruns = size(ARGS)[1] > 1 ? integer(ARGS[2]) : 25
end

N = isdefined(:N) ? N : 5
nruns = isdefined(:nruns) ? nruns : 25

# seed the RNG
# FIXME: seed from /dev/urandom instead?
srand(integer(time()));

# scan/cumulative sum
h_data = rand(Float64, N);

# warm cache
h_sum = mapreduce(x->x^2, +, h_data);

tic()
for i=1:nruns
    mapreduce(x->x^2, +, h_data);
end
reduce_time = toc();

println("transform-reduce gflops: ",2*N/reduce_time/nruns/1e6)

sum_cpp = dot(h_data, h_data);
println("Residual: ", (h_sum - sum_cpp)/sum_cpp)
