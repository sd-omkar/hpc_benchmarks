#include "multi_row_slice.decl.h"
#include "MultiRowSlice.h"
#include "main.decl.h"
#include "DoubleArray.h"

extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ DoubleArray x;

MultiRowSlice::MultiRowSlice()
{
}

MultiRowSlice::MultiRowSlice(CkMigrateMessage *msg)
{
}

MultiRowSlice::~MultiRowSlice()
{
	if (_nnz > 0)
	{
		delete[] _myRes;
		delete[] _vals;
		delete[] _col_idx;
		delete[] _row_start;
	}
}

void MultiRowSlice::init(int nRows, int nnz, int firstRow, int firstIdx, double *vals, int *col_idx, int *row_start)
{
	//CkPrintf("MULT_SLICE chare %d.init() called.\n", thisIndex);
	int j;

	//CkPrintf("CHARE %d INIT CALLED WITH nRows=%d, nnz=%d, firstRow=%d, firstVal=%d\nvals=",
	//	thisIndex, nRows, nnz, firstRow, firstIdx);
	//for (j = 0; j < nnz; j++) CkPrintf(" %f", vals[j]);
	//CkPrintf("\ncol_idx=");
	//for (j = 0; j < nnz; j++) CkPrintf(" %d", col_idx[j]);


	_nRows = nRows;
	_nnz = nnz;
	_firstRow = firstRow;

	/*CkPrintf("Chare %d allocating local fields. nnz=%d, nRows=%d\n", thisIndex, nnz, nRows);
	CkExit();*/

	_vals = new double[nnz];
	_col_idx = new int[nnz];
	_row_start = new int[nRows + 1];
	_myRes = new double[nRows];

	//CkPrintf("Ok.\n");

	for (j = 0; j < nnz; j++)
	{
		_vals[j] = vals[j];
		_col_idx[j] = col_idx[j];
	}
	for (j = 0; j < nRows; j++)
	{
		_myRes[j] = 0.;
	}

	_row_start[0] = 0;
	_row_start[nRows] = nnz;
	for (j = 1; j < nRows; j++)
	{
		_row_start[j] = row_start[j - 1] - firstIdx;
	}


	//CkPrintf("\nlocal_row_start="); 
	//for (j = 0; j < nRows + 1; j++) CkPrintf(" %d", _row_start[j]);
	//CkPrintf("\n");

	::mainProxy.stageFinished();
}

void MultiRowSlice::calc()
{
	/*
	 * Basically this is a CSR SpMV over a sparse submatrix.
	 */
	int i, j;

	for (i = 0; i < _nRows; i++)
	{
		_myRes[i] = 0.;
		for (j = _row_start[i]; j < _row_start[i + 1]; j++)
		{
			_myRes[i] += _vals[j] * x[_col_idx[j]];
		}
	}

	::mainProxy.setResultMultiRowSlice(_nRows, _firstRow, _myRes);
}

#include "multi_row_slice.def.h"