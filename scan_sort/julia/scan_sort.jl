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
srand(integer(time()));

# scan/cumulative sum
# gen random ints
h_scan = rand(Int32, N);

# warm cache
cumsum(h_scan)

tic()
for i=1:nruns
    h_scan = cumsum(h_scan);
end
scan_time = toc();

println("scan keys/s: ",(nruns*N)/scan_time)


# sort
h_sort = rand(Int64, N);

# warm cache
sort(h_sort)

tic()
for i=1:nruns
    sort(h_sort)
end
sort_time = toc();

println("sort keys/s: ",(nruns*N)/sort_time)
