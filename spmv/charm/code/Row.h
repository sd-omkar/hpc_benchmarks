#ifndef __ROW_H_
#define __ROW_H_

class Row : public CBase_Row
{
public:
	Row();
	Row(CkMigrateMessage *msg);
	~Row();

	void calc();
	void init(int num, int nnz, double *vals, int *col_idx);

private:
	int _rowNum; /* row number in global matrix */
	int _nnz;	 /* number of nonzeros in this row */

	double *_vals; /* nonzero value array for this row */
	int *_col_idx; /* column indices for this row's entries */
	double _myRes; /* result y[_rowNum] from y=A*x */
};

#endif //__ROW_H_