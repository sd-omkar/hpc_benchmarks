#include <algorithm>
#include <fstream>
#include <cmath>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#include <cusp/io/matrix_market.h>
#include <cusp/csr_matrix.h>
#include <cusp/multiply.h>
#include <cusp/blas.h>

#include <timer.h>

#include "cusparse.h"


// -----------------------------------------------------------------------------
// Macro to obtain a random number between two specified values
// -----------------------------------------------------------------------------
#define RAND(L,H)  ((L) + ((H)-(L)) * (float)rand()/(float)RAND_MAX)


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef WIN32
#   define ISNAN(A)  (_isnan(A))
#else
#   define ISNAN(A)  (isnan(A))
#endif


// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------
typedef double REAL;
typedef double PREC_REAL;

typedef typename cusp::csr_matrix<int, REAL, cusp::device_memory> Matrix;
typedef typename cusp::array1d<REAL, cusp::device_memory>         Vector;
typedef typename cusp::array1d<REAL, cusp::host_memory>           VectorH;
typedef typename cusp::array1d<PREC_REAL, cusp::device_memory>    PrecVector;


// -----------------------------------------------------------------------------
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::string;
using std::vector;

void spmv(int n, int nnz, const int *row_offsets, const int *column_indices, const float *values,
		  cusparseHandle_t& handle, cusparseMatDescr_t& descrA,
		  const float *x, float *y);

void spmv(int n, int nnz, const int *row_offsets, const int *column_indices, const double *values,
		  cusparseHandle_t& handle, cusparseMatDescr_t& descrA,
		  const double *x, double *y);

// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------
int main(int argc, char** argv) 
{
	// Set up the problem to be solved.
	string         fileMat;
	if (argc < 2) {
		cerr << "Usage: ./driver_cusparse_csr ${MATRIX_FILE_NAME}" << endl;
		exit(-1);
	}

	fileMat = argv[1];

	cout << fileMat << endl;

	// Get matrix and rhs.
	Matrix A;
	Vector b;
	Vector x;

	cusp::io::read_matrix_market_file(A, fileMat);

	b.resize(A.num_rows);

	{
		VectorH x_h(A.num_rows);

		for (int i = 0; i < A.num_rows; i++)
			x_h[i] = RAND(2,10) / 2;

		x = x_h;
	}

	cusparseHandle_t        handle;
	cusparseMatDescr_t      descrA;

	cusparseCreate(&handle);
	cusparseCreateMatDescr(&descrA);

	cusparseSetMatType(descrA,      CUSPARSE_MATRIX_TYPE_GENERAL);
	cusparseSetMatDiagType(descrA,  CUSPARSE_DIAG_TYPE_NON_UNIT);
	cusparseSetMatIndexBase(descrA, CUSPARSE_INDEX_BASE_ZERO);
	cusparseSetMatFillMode(descrA,  CUSPARSE_FILL_MODE_LOWER);

	CUDATimer timer;
	int counter = 0;
	double elapsed = 0.0;
	for (int i = 0; i < 10; i++) {
		timer.Start();
		spmv(A.num_rows, A.num_entries, thrust::raw_pointer_cast(&A.row_offsets[0]), thrust::raw_pointer_cast(&A.column_indices[0]), 
			 thrust::raw_pointer_cast(&A.values[0]), handle, descrA, thrust::raw_pointer_cast(&x[0]), thrust::raw_pointer_cast(&b[0]));
		timer.Stop();

		if (i > 0) {
			counter ++;
			elapsed += timer.getElapsed();
		}
	}
	elapsed /= counter;
	cout << "cuSparse CSR: " << elapsed << endl;

	cusparseDestroyMatDescr(descrA);
	cusparseDestroy(handle);

	return 0;
}

void spmv(int n, int nnz, const int *row_offsets, const int *column_indices, const float *values,
		  cusparseHandle_t& handle, cusparseMatDescr_t& descrA,
		  const float *x, float *y)
{
	float one = 1.f, zero = 0.f;
    cusparseScsrmv(handle, CUSPARSE_OPERATION_NON_TRANSPOSE, n, n, nnz, &one, descrA, values, row_offsets, column_indices, x, &zero, y);
}

void spmv(int n, int nnz, const int *row_offsets, const int *column_indices, const double *values,
		  cusparseHandle_t& handle, cusparseMatDescr_t& descrA,
		  const double *x, double *y)
{
	double one = 1.0, zero = 0.0;
    cusparseDcsrmv(handle, CUSPARSE_OPERATION_NON_TRANSPOSE, n, n, nnz, &one, descrA, values, row_offsets, column_indices, x, &zero, y);
}
