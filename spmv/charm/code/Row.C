#include "row.decl.h"
#include "Row.h"
#include "main.decl.h"
#include "DoubleArray.h"

extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ DoubleArray x;

Row::Row() : _nnz(0)
{

}

Row::Row(CkMigrateMessage *msg)
{

}

Row::~Row()
{
	if (_nnz > 0)
	{
		delete[] _vals;
		delete[] _col_idx;
	}
}

void Row::init(int num, int nnz, double *vals, int *col_idx)
{
	int j;
	
	_rowNum = num;
	_nnz = nnz;
	_vals = new double[nnz];
	_col_idx = new int[nnz];

	for (j = 0; j < nnz; j++)
	{
		_vals[j] = vals[j];
		_col_idx[j] = col_idx[j];
	}
	
	::mainProxy.stageFinished();
}

void Row::calc()
{
	int j;
	_myRes = 0.;

	for (j = 0; j < _nnz; j++)
	{
		_myRes += _vals[j] * x[_col_idx[j]];
	}

	::mainProxy.setResultNoSlice(_rowNum, _myRes);
}

#include "row.def.h"