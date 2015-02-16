#ifndef __MAIN_H_
#define __MAIN_H_

enum Stages
{
	STAGE_INIT,
	STAGE_CALCULATE,
	STAGE_DONE,
	STAGE_QUIT
};

enum Mode
{
	NO_SLICE,
	SINGLE_SLICE,
	MULTI_SLICE
};

class Main : public CBase_Main
{
public:
	Main(CkArgMsg* msg);
	Main(CkMigrateMessage* msg);
	~Main();

	void finalize();
	void stageFinished();
	void setResult(int row, double res);
	void setResultMultiRow(int nRows, int firstRow, double *res);


private:
	void nextStage();

	CProxy_MultiRowSlice _multiRowSliceArray;
	CProxy_RowSlice _sliceArray;
	CProxy_Row _rowArray;

	int _N, _nnz, _totalChares;
	int _charesFinished;
	int _curStage;
	int _runs, _runsFinished;
	int _sliceMode;

	double _tStart;
	double *_y;

	char _inputFile[256], _vectorInput[256];
};

#endif //__MAIN_H_
