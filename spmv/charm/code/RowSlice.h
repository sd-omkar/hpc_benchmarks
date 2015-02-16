#ifndef __ROW_SLICE_H_
#define __ROW_SLICE_H_

#include "row_slice.decl.h"

class RowSlice : public CBase_RowSlice
{
public:
	RowSlice();
	RowSlice(CkMigrateMessage *msg);
	~RowSlice();

	void calc();
	void init(int num, int nnz, double *vals, int *col_idx);
	void initBlank(int num); /* just save the row number and
							    report back to main chare that
								we're ready as nothing needs
								to be initialized. */

private:
	int _rowNum; /* row number in global matrix */
	int _nnz;	 /* number of nonzeros in this row */

	double *_vals; /* nonzero value array for this row */
	int *_col_idx; /* column indices for this row's entries */
	double _myRes; /* result y[_rowNum] from y=A*x */
};

#endif //__ROW_SLICE_H_