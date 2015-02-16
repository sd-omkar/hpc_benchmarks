#ifndef __MULTI_ROW_SLICE_H_
#define __MULTI_ROW_SLICE_H_

/*
 *
 * Initialization input:
 *		- int nRows					(number of rows this slice spans over. NOTE: Must be consecutive rows!)
 *		- int nnz					(total number of nonzero entries in this slice)
 *		- int firstRow				(global row index of the first nonzero entry)
 *		- int firstIdx				(index of first entry in global value/col_idx array)
 *
 *		- double vals[nnz]			(values of nonzero entries)
 *		- int col_idx[nnz]			(column indices of nonzero entries)
 *		- int row_start[nRows]		(global indices in col_idx array where row breaks occur)
 *
 *
 * Calculation output (sent back to MainChare::setResult):
 *		- int _nRows				(length of results array)
*		- int _firstRow				("row offset" for result vector _myRes)
 *		- double _myRes[_nRows]		(result values for each 'touched' row)
 *
 */

class MultiRowSlice : public CBase_MultiRowSlice
{
public:
	MultiRowSlice();
	MultiRowSlice(CkMigrateMessage *msg);
	~MultiRowSlice();

	void calc();
	void init(int nRows, int nnz, int firstRow, int firstIdx, double *vals, int *col_idx, int *row_start);

private:
	//int *_rowNums; /* row numbers in global matrix; one per nnz entry */
	int _nnz;	 /* number of nonzeros in this slice */
	int _nRows;  /* number of rows this slice spans over */
	int _firstRow; /* global row index of first nonzero entry, "offset" for result vector */

	double *_vals; /* nonzero value array for this row */
	int *_col_idx; /* column indices for this slice's entries */
	int *_row_start; /* indices in column index array where row changes occur */
	double *_myRes; /* results y[i] for all i from y=A*x */
};

#endif //__MULTI_ROW_SLICE_H_