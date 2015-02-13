#ifndef __MAIN_H_
#define __MAIN_H_

enum Stages
{
	STAGE_INIT,
	STAGE_CALCULATE,
	STAGE_DONE,
	STAGE_QUIT
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


private:
	void nextStage();

	CProxy_RowSlice _sliceArray;
	CProxy_Row _rowArray;

	int _N, _nnz, _totalChares;
	int _charesFinished;
	int _curStage;
	int _runs, _runsFinished;

	double _tStart;
	double *_y;

	bool _useSlice;
	char _inputFile[256];
};

#endif //__MAIN_H_
