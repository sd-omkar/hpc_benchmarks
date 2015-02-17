# Charm++ SpMV implementation #

----------
D.Heller Feb' 2015

dominik.heller1@gmail.com

## Overview ##

This code comprises an implementation of Sparse Matrix-Vector multiply using CSR matrix format.

Matrices can be read in COO-/matrix market ASCII format (`-i input.mtx`), in a binary pure CSR format (`-I input.csr`) or generated randomly with a given size and number of nonzeros (`-r -N rows -e entries`). Use `-k` if you want to keep the generated matrix, which will be saved in CSR format.

Operand vectors will be generated randomly by default or can be specified as a binary input file (`-x vector.vec`).
The script `convert.py` allows converting COO-matrices or ASCII vector files to the corresponding binary format.

In Charm++ the parallel program flow is realized with chares (C++ objects) running on different CPUs or nodes. Thus, the matrix has to be distributed to several chares.
The present implementation allows three different strategies for this:

- **No slicing** (default):
One chare represents exactly one row in the matrix.
- **Row slicing** (`-s sliceSize`):
Each row is distributed over several chares with a given maximum number of nonzero-entries *sliceSize*. In theory, for sliceSize=1, you would have one chare for each and every entry in the matrix.
- **Multi row slicing** (`-m sliceSize`):
Each chare holds a fixed amount of *sliceSize* consecutive nonzero-entries, regardless of over how many rows these values are scattered in the matrix.


## Command line options ##

Usage:

- Use binary CSR input: `spmv -I input_bin.csr`      [`opts`] [`slice`]
- Use ASCII COO input: `spmv -i input_ascii.mtx`    [`opts`] [`slice`]
- Generate random matrix: `spmv -r -N rows -e entries` [`opts`] [`slice`]
- [opts]
  * Do multiple runs and display FLOPS: `-n runs`
  * Specify operand vector instead of random one (binary only): `-x vector.vec`
  * Save randomly generated matrix in CSR-format (`-r` only): `-k`
- [slice]
  To specify that each chare shall hold at most *size* values, use
  * Row slicing: `-s size`
  * Multi row slicing: `-m size`
  * To specify a maximum total number of chares: `-S maxChares`


**Note:** Using COO input is currently not correctly implemented as actually the transposed matrix is read and used for calculation.
For unsymmetric matrices, COO input will yield wrong results.
Please use CSR input whenever possibly, you can use `convert.py` for easy conversion.


## Benchmarking ##

A very simple benchmarking script `bench.py` is provided.
Feel free to use and adjust to your needs.
Please note that FLOPS are only displayed for *-n runs* &ge; 2.


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
This binary format reflects the internal memory representation of a CSR matrix and thus can be read quickly and comprises small file sizes.

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
- `x[i]` (i = 1, ..., `N`), vector entries

You can convert ASCII vectors, provided they are just doubles separated by whitespaces, using `convert.py -v input output`.


### `convert.py`: ###

This script allows conversion from COO matrices to the more efficient CSR format, as well as converting ASCII vectors to binary representation.

- Convert matrices: `[python] convert.py input output`
- Convert vectors: `[python] convert.py -v input output`

`input` may contain wildcards and `output` may be a folder.

Unlike when using the COO format directly as an input file for `spmv`, matrices converted by `convert.py` *will not* be transposed, they produce correct results.