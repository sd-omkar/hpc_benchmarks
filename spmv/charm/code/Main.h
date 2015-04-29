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
	UNKNOWN =			0,
	NO_SLICE =			1 << 0,
	ROW_SLICE =			1 << 1,
	MULTI_SLICE =		1 << 2,
	VALIDATE =			1 << 8
};

class Main : public CBase_Main
{
public:
	Main(CkArgMsg* msg);
	Main(CkMigrateMessage* msg);
	~Main();

	void finalize();
	void stageFinished();
	void setResultNoSlice(int row, double res);
	void setResultRowSlice(int row, double res);
	void setResultMultiRowSlice(int nRows, int firstRow, double *res);


private:
	void compareResults(bool forceOutput=false);
	void nextStage();
	void nullResultArrays();

	CProxy_MultiRowSlice _multiRowSliceArray;
	CProxy_RowSlice _sliceArray;
	CProxy_Row _rowArray;

	int _N, _nnz, _totalChares;
	int _charesFinished;
	int _curStage;
	int _runs, _runsFinished;
	int _sliceMode;

	double _tStart;
	double *_yNoSlice, *_yRowSlice, *_yMultiRowSlice;
	double *_ySeq;

	char _inputFile[256], _vectorInput[256];
};

#endif //__MAIN_H_
