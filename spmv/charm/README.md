# Charm++ SpMV #

----------
D.Heller - February 2015 - dominik.heller1@gmail.com

## Overview ##

This code comprises an implementation of Sparse Matrix-Vector multiply using the CSR matrix format.

Matrices can be read in COO-/matrix market ASCII format (`-i input.mtx`), in a binary pure CSR format (`-I input.csr`) or generated randomly with a given size and number of nonzeros (`-r -N rows -e entries`). Use `-k` if you want to keep the generated matrix, which will be saved in CSR format.

Operand vectors will be generated randomly by default or can be specified as a binary input file (`-x vector.vec`).
The script `convert.py` allows converting COO-matrices or ASCII vector files to the corresponding binary format.

In Charm++ the parallel program flow is realized with chares (C++ objects) running on different CPUs or nodes. Thus, the matrix has to be distributed to several chares.
The present implementation allows three different strategies for this:

- **No slicing** (default):
One chare represents exactly one row in the matrix, regardless of how many entries are in that row.
- **Row slicing** (`-s sliceSize`):
Each row is distributed over several chares with a given maximum number of nonzero-entries *sliceSize*. In theory, for sliceSize=1, you would have one chare for each and every entry in the matrix.
- **Multi row slicing** (`-m sliceSize`):
Each chare holds a fixed amount of *sliceSize* consecutive nonzero-entries, regardless of over how many rows these values are scattered in the matrix.


## Getting Charm++

The following steps should set you up:

`git clone http://charm.cs.uiuc.edu/gerrit/charm`  
`git checkout charm-6.6.1`  
`./build charm++ multicore-linux64 --with-production` (in charm directory)

This will build a single-node version of Charm++. Check the documentation if you need other architectures, compile options, or anything else.

## Compile ##

Just edit the Charm++ path in the `Makefile`.  
Afterwards, `make` should do the trick.


## Command line options ##

Usage of the `spmv` program:

- Use binary CSR input: `spmv -I input_bin.csr`      [`opts`] [`slice`]
- Use ASCII COO input: `spmv -i input_ascii.mtx`    [`opts`] [`slice`]
- Generate random matrix: `spmv -r -N rows -e entries` [`opts`] [`slice`]
- [opts]
  * Do multiple runs and display FLOPS: `-n runs`
  * Specify operand vector instead of random one (binary only): `-x vector.vec`
  * Save randomly generated matrix in CSR-format (`-r` only): `-k`
  * Validate results by comparing all slice methods to the sequential result: `-v`
    (High memory requirement!)
- [slice]
  To specify that each chare shall hold at most *size* values, use
  * Row slicing: `-s size`
  * Multi row slicing: `-m size`
  * **CURRENTLY IGNORED** --- To specify a maximum total number of chares: `-S maxChares`

**charmrun:** To run the program on more than one CPU, you must use the `charmrun` environment (roughly comparable to `mpirun`).
For example, to use on all CPUs on a 16 core SMP node, run `./charmrun +p16 ./spmv ARGS` where *ARGS* are any of the aforementioned arguments for `spmv`.
Check the Charm++ documentation for further functionality.


**Note:** Using COO input is currently not correctly implemented as actually the transposed matrix is read and used for calculation.
For unsymmetric matrices, COO input will yield wrong results.
Please use CSR input whenever possibly, you can use `convert.py` for easy conversion.

## Examples ##

- Generate a randomized 1,000x1,000 matrix with 100,000 nonzero entries and store it in CSR format: `spmv -r -N 1000 -e 100000 -k`
- Validate results: `spmv -I rnd1000_100000.csr -v`
- Do 10,000 test runs on 16 cores: `charmrun +p16 ./spmv -I rnd1000_100000.csr -n 10000`
- Do test runs using multi row splicing: `spmv -I rnd1000_100000 -n 10000 -m 12500` (compare MFlops!)
- Multiply matrix `small4x4.mtx` with vector `1_4.vec` (results will be displayed for very small systems): `spmv -i small4x4.mtx -x 1_4.vec`
- Convert an ASCII COO matrix to binary CSR representation: `python convert.py my_large_matrix.mtx my_csr_matrix.csr`
- Convert any number of COO matrices: `python convert.py input/*.mtx input_bin/`


## Benchmarking ##

Two very simple benchmarking scripts `bench.py`, `bench2.py` are provided.
Feel free to use and adjust to your needs.
Please note that Flops are only displayed for *-n runs* &ge; 2.

In addition, the script `parselog.py` allows to parse `spmv`'s output for MFlops results, matrix dimension metrics and other statistics, which can be exported to a CSV file for convenient viewing in Excel. See the comments in the code for information on how to use `parselog` exactly.

**Sample benchmarking process:**  
	1. Deploy some input matrices anywhere and make a subfolder `bench` in the directory where `spmv` is.  
	2. Edit `bench2.py` to have the list of input files in variable `inFiles`.  
	3. Run `bench2.py`, logs will be written to subfolder `bench`.  
	4. Run `parselog.py > results.csv` to save statistics, open in Excel.


## Known issues ##

* Currently, the result vector is not saved in any way. Vectors for small systems (*N* < 20) are written to stdout.
* Be wary about memory requirements, especially with `-s` single row slicing and `-v` validation mode. For very large matrices with low density, try to use `-m` multi row slicing, which is the best performing method anyway.
* Inappropriate choice of slice sizes may lead to bad_allocs and other problems, you might need to experiment. For a start, try `-m 1024` and increase to up to 12500 or until segfaults occur. Higher values usually increase performance, as long as NNZ >> slice size.
I'm not sure about these segfaults, there might be a bug. Chares should be able to handle more than 12500 entries in theory.
* Randomized matrices are really random. They aren't structured in any way and usually have one very densely filled row at the end. Using real-world matrices for serious benchmarking is advised.


## File formats ##

This program uses very simple parsers and expects data to be provided precisely in the correct format.

### COO-/ASCII input (`-i`): ###
The parser will open input files in text mode and read multiple integer and floating point numbers, separated by whitespaces, in the following order.

- `ncols` number of columns (*int*)
- `nrows` number of rows (*int*)
- `nnz` number of nonzero entries (*int*)
- for k in {1, ..., nnz}:
  * `col` column index (1-based, *int*)
  * `row` row index (1-based, *int*)
  * `value` entry value (*double*)

No comments are allowed and it is assumed that the matrix is quadratic and contains at least one non-zero entry per row.

Please avoid using COO input when possible, as i) in fact the transposed matrix will be read and processed (see above), ii) ASCII files are large in size and slow to read. Use `convert.py` for easy and correct conversion to CSR format.

### CSR-/binary input (`-I`): ###
This binary format reflects the internal memory representation of a CSR matrix and thus can be read quickly and features small file sizes.

The parser opens input files in binary mode and reads the following values in this order:

- `N` number of rows (*int*)
- `nnz` number of nonzero entries (*int*)
- `value[k]` (k = 1, ..., `nnz`),  entry value (*double*)
- `col_idx[k]` (k = 1, ..., `nnz`),  entry column index (*int*)
- `row_start[i]` (i = 1, ..., `N`+1),  indices where the next row begins (*int*)

For validation, check that your input files are exactly [12 * `nnz` + 4 * (`N`+1)] bytes long (provided integers of 4 and doubles of 8 bytes are used).


### Vector input (`-x`): ###

Operand vectors can currently only be read in a dense binary format.

Expected values and their order are as follows:

- `N` size of vector (*int*)
- `x[i]` (i = 1, ..., `N`), vector entries (*double*)

You can convert ASCII vectors, provided they are just doubles separated by whitespaces, using `convert.py -v input output`.


### `convert.py`: ###

This script allows conversion from COO matrices to the more efficient CSR format, as well as converting ASCII vectors to binary representation.

- Convert matrices: `[python] convert.py input output`
- Convert vectors: `[python] convert.py -v input output`

`input` may contain wildcards and `output` may be a folder.

Unlike when using the COO format directly as an input file for `spmv -i`, matrices converted by `convert.py` will *not* be transposed, they produce correct results.