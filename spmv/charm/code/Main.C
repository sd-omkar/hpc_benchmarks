#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "main.decl.h"
#include "Main.h"
#include "DoubleArray.h"

using namespace std;

/* readonly */ CProxy_Main mainProxy;
/* readonly */ DoubleArray x;

void generateRandom(int nnz, int N, int &rowMaxNnz, double *&vals, int* &col_idx, int *&row_start, bool keepMatrix)
{
	int i, j, count, countThisRow, toFill;
	double rnd, chance;
	srand(time(NULL));
	
	CkPrintf("Creating random matrix with %d rows and %d nonzero entries.\n", N, nnz);
	CkAssert(nnz >= N);

	// allocate arrays
	vals = new double[nnz];
	col_idx = new int[nnz];
	row_start = new int[N + 1];

	CkPrintf("Memory for CSR matrix:           %d KB.\n", (sizeof(double)*nnz + sizeof(int)*nnz + sizeof(int)*(N + 1)) / 1024);
	CkPrintf("Memory for vector (per CPU):     %d KB.\n", (sizeof(double)*N) / 1024);

	chance = 1.0 * (nnz-N) / N / N;
	count = 0;
	rowMaxNnz = 0;
	for (i = 0; i < N; i++)
	{
		countThisRow = 0;
		row_start[i] = count;
		for (j = 0; j < N; j++)
		{
			if (count >= nnz) break;

			long indicesToCome = (long)N*N - (i*N + j);
			if ((nnz-count >= indicesToCome)	// we're short, every entry has to be filled
				|| (i==j)						// fill diagonal
				|| ( count+(N-i) < nnz && 1.*rand() / RAND_MAX <= chance)) // only fill arbitrary entries if enough NNZ are left for diagonals
			{
				do
				{
					rnd = 2.*rand() / RAND_MAX - 1.;
				}
				while (fabs(rnd) < 1.e-12);

				vals[count] = rnd;
				col_idx[count] = j;
				++count;
				++countThisRow;
				//CkPrintf("Inserting value %f at (%d, %d).\n", rnd, i, j);
			}
		}
		if(i%1000==0) CkPrintf("row: %d\n", i);
		CkAssert(countThisRow >= 1);
		if (countThisRow > rowMaxNnz) rowMaxNnz = countThisRow;
	}
	row_start[N] = count;

	CkPrintf("Number of nonzeros: %d\n", count);
	CkAssert(count == nnz);

	/*
	 * Save to disk (binary CSR format)
	 */
	if (keepMatrix)
	{
		char filename[256];
		FILE *f = NULL;

		sprintf(filename, "rnd%d_%d.csr", N, nnz);
		CkPrintf("Saving matrix as '%s'.\n", filename);

		f = fopen(filename, "wb");
		if (f == NULL) return;

		fwrite(&N, sizeof(int), 1, f);
		fwrite(&nnz, sizeof(int), 1, f);
		fwrite(&vals[0], sizeof(double), nnz, f);
		fwrite(&col_idx[0], sizeof(int), nnz, f);
		fwrite(&row_start[0], sizeof(int), N+1, f);
		fclose(f);
	}
}

void csrImport(char* inFile, int &nnz, int &N, int &rowMaxNnz, double* &vals, int* &col_idx, int* &row_start)
{
	int i, res;
	FILE *f = NULL;

	CkPrintf("Reading binary input file '%s'.\n", inFile);

	f = fopen(inFile, "rb");
	if (f == NULL)
	{
		CkPrintf("FATAL: Could not open file '%s'. Terminating.\n", inFile);
		CkExit();
	}

	if (1 != fread(&N, sizeof(int), 1, f)
		|| 1 != fread(&nnz, sizeof(int), 1, f))
	{
		CkPrintf("FATAL: Could not read N or nnz. Terminating.\n");
		CkExit();
	}
	CkAssert(N >= nnz);
	CkAssert(N > 0);
	CkAssert(nnz > 0);
	CkAssert((long)nnz <= (long)N*(long)*N);

	/*
	 * Allocate arrays
	 */

	vals = new double[nnz];
	col_idx = new int[nnz];
	row_start = new int[N+1];

	res = fread(&vals[0], sizeof(double), nnz, f);
	if (res!=nnz)
	{
		CkPrintf("FATAL: Could not read %d values. Read %d. Terminating.\n", nnz, res);
		CkPrintf("Expected %d values/col_idxs and %d row_starts.\n", nnz, N + 1);
		CkExit();
	}
	res = fread(&col_idx[0], sizeof(int), nnz, f);
	if (res!=nnz)
	{
		CkPrintf("FATAL: Could not read %d col_idx's. Read %d. Terminating.\n", nnz, res);
		CkPrintf("Expected %d values/col_idxs and %d row_starts.\n", nnz, N + 1);
		CkExit();
	}
	res = fread(&row_start[0], sizeof(int), N + 1, f);
	if (res!=N+1)
	{
		CkPrintf("FATAL: Could not read %d row_start's. Read %d. Terminating.\n", N+1, res);
		CkPrintf("Expected %d values/col_idxs and %d row_starts.\n", nnz, N + 1);
		CkExit();
	}
	fclose(f);

	rowMaxNnz = 0;
	for (i = 0; i < N; i++)
		if (row_start[i + 1] - row_start[i] > rowMaxNnz)
			rowMaxNnz = row_start[i + 1] - row_start[i];

	CkPrintf("Successfully read '%s'.\n%d rows, %d nonzeroes, %d max nonzeroes per row.\n", inFile, N, nnz, rowMaxNnz);
}

void sloppyMmImport(char* inFile, int &nnz, int &N, int &rowMaxNnz, double* &vals, int* &col_idx, int* &row_start)
{
	// this will actually import the transposed matrix
	// as MM files are written column-wise but we need
	// row-wise representation.

	int i, k, entry;
	int nrows, ncols;
	int col, row, lastRow;
	int thisRowNnz;
	double value;

	ifstream f;
	CkPrintf("Reading input file '%s'.\n", inFile);
	f.open(inFile);
	if (f.fail())
	{
		CkPrintf("FATAL: Input file could not be read. Terminating.\n");
		CkExit();
	}

	f >> ncols; // should be nrows
	f >> nrows; // should be ncols
	f >> nnz;
	N = nrows;

	// allocate arrays
	vals = new double[nnz];
	col_idx = new int[nnz];
	row_start = new int[N+1];

	CkPrintf("Memory for CSR matrix:           %d KB.\n", (sizeof(double)*nnz + sizeof(int)*nnz + sizeof(int)*(N+1)) / 1024);
	CkPrintf("Memory for vector (per CPU):     %d KB.\n", (sizeof(double)*N)/1024);

	lastRow = -1;

	thisRowNnz = 0;
	rowMaxNnz = 0;
	for (k = 0; k < nnz; k++)
	{
		f >> col; --col; // should be row
		f >> row; --row; // should be col ALL ROWS COLS START AT 1 IN THE INPUT FILE
		f >> value;

		vals[k] = value;
		col_idx[k] = col;

		if (row > lastRow)
		{
			lastRow = row;
			row_start[row] = k;
			if (thisRowNnz > rowMaxNnz) rowMaxNnz = thisRowNnz;
			thisRowNnz = 1;
		}
		else ++thisRowNnz;
	}
	row_start[row + 1] = k; // ?? muss wohl..
	f.close();
}

int getOptions(int argc, char** argv, bool &randomize, bool &keepMatrix, bool &binary, bool &randomVector,
	int &N, int &nnz, int &runs, char *inputFile, char *vectorInput, int &sliceSizeRow, int &sliceSizeMult, int &maxSlices,
	int &mode)
{
	int c;

	/*
	 * Defaults
	 */
	keepMatrix = false;
	randomVector = true;
	bool readFile = false;
	bool useSingleSlice = false;
	bool useMultiSlice = false;
	bool validate = false;

	while ((c = getopt(argc, argv, "ri:I:N:e:s:S:kn:x:m:v")) != -1)
	{
		switch (c)
		{
		case 'r':
			randomize = true;
			break;
		case 'i':
			readFile = true;
			binary = false;
			strncpy(inputFile, optarg, 256);
			break;
		case 'I':
			readFile = true;
			binary = true;
			strncpy(inputFile, optarg, 256);
			break;
		case 'N':
			N = atoi(optarg);
			break;
		case 'e':
			nnz = atoi(optarg);
			break;
		case 's':
			sliceSizeRow = atoi(optarg);
			useSingleSlice = true;
			mode |= ROW_SLICE;
			break;
		case 'k':
			keepMatrix = true;
			break;
		case 'n':
			runs = atoi(optarg);
			break;
		case 'x':
			randomVector = false;
			strncpy(vectorInput, optarg, 256);
			break;
		case 'm':
			sliceSizeMult = atoi(optarg);
			useMultiSlice = true;
			mode |= MULTI_SLICE;
			break;
		case 'v':
			mode |= VALIDATE | NO_SLICE | ROW_SLICE | MULTI_SLICE;
			break;
		case '?':
		default:
			CkPrintf("Usage: %s -i input(ascii) [opts] [slice] OR\n"
				"       %s -I input(binary) [opts] [slice] OR \n"
				"       %s -r -N size -e entries [opts] [slice] \n\n"
				"[opts] can be:   -n number of runs\n"
				"                 -x input file to operand vector\n"
				"                 -k keep generated matrix in file\n"
				"                 -v validate results by testing all slice methods (high memory usage)\n"
				"[slice] can be one of\n"
				"                 -s slice size (Single row slicing)\n"
				"             OR  -m slice size (Multi row slicing)\n"
				//" and optionally  -S max slices (both variants)\n"
				, argv[0], argv[0], argv[0]);

			CkPrintf("Error parsing option: '-%s'\n", &c);
			return EXIT_FAILURE;
		}
	}

	if (!(useMultiSlice || useSingleSlice) || mode&VALIDATE)
		mode |= NO_SLICE;

	/* check validity */
	if (randomize && readFile
		|| !(randomize || readFile)
		|| (randomize && (N == 0 || nnz == 0))
		|| (!(mode&VALIDATE) && useMultiSlice && useSingleSlice))
	{
		CkPrintf("Usage: %s -i input(ascii) [opts] [slice] OR\n"
			"       %s -I input(binary) [opts] [slice] OR \n"
			"       %s -r -N size -e entries [opts] [slice] \n\n"
			"[opts] can be:   -n number of runs\n"
			"                 -x input file to operand vector\n"
			"                 -k keep generated matrix in file\n"
			"                 -v validate results by testing all slice methods (high memory usage)\n"
			"[slice] can be one of\n"
			"                 -s slice size (Single row slicing)\n"
			"             OR  -m slice size (Multi row slicing)\n"
			//" and optionally  -S max slices (both variants)\n"
			, argv[0], argv[0], argv[0]);

		CkPrintf("Error: Invalid options!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// entry point of application
Main::Main(CkArgMsg* msg)
{
	int i, j;

	/*
	 * Default parameters and settings
	 */
	int sliceSizeRow = 1024, sliceSizeMult = 2048;
	int maxSlices = 250000; // currently ignored!!
	bool randomize = false;
	
	_runs = 1;
	_sliceMode = UNKNOWN;

	double *vals;
	double density;
	int *col_idx, *row_start;
	int rowMaxNnz;
	bool randomVector;
	bool keepMatrix;
	bool binInput=false;

	_ySeq = NULL;

	/*
	 * Read options
	 */
	_N = 0;
	_nnz = 0;
	if (EXIT_FAILURE == getOptions(msg->argc, msg->argv, randomize, keepMatrix, binInput, randomVector,
		_N, _nnz, _runs, _inputFile, _vectorInput, sliceSizeRow, sliceSizeMult, maxSlices, _sliceMode))
		CkExit();
	delete msg;

	/*
	 * Init matrix
	 */
	if (randomize)
		generateRandom(_nnz, _N, rowMaxNnz, vals, col_idx, row_start, keepMatrix);
	else if (binInput)
		csrImport(_inputFile, _nnz, _N, rowMaxNnz, vals, col_idx, row_start);
	else
		sloppyMmImport(_inputFile, _nnz, _N, rowMaxNnz, vals, col_idx, row_start);

	density = 1.*_nnz / _N / _N;

	if (density < 1.e-4) CkPrintf("Density: %E\n", density);
	else CkPrintf("Density: %.2f%%\n", 100.*density);
	CkPrintf("Max. number of nonzeroes per row: %d (avg. %d)\n", rowMaxNnz, _nnz/_N);
	if (_N < 20)
	{
		CkPrintf("vals: ");
		for (i = 0; i < _nnz; i++)
			CkPrintf(" %f", vals[i]);
		CkPrintf("\ncol_i:");
		for (i = 0; i < _nnz; i++)
			CkPrintf(" %d", col_idx[i]);
		CkPrintf("\nrow_s:");
		for (i = 0; i < _N+1; i++)
			CkPrintf(" %d", row_start[i]);
		CkPrintf("\n");
	}

	/* 
	 * Init vector
	 */
	bool readNonNull = false;

	::x.resize(_N);
	if (!randomize && randomVector)
	{
		srand(time(NULL));
		for (i = 0; i < _N; i++)
			x[i] = double(1.* rand() / RAND_MAX + 1.0e-10); // randomly init x
	}
	else if (!randomVector)
	{
		FILE* vecFile = NULL;
		vecFile = fopen(_vectorInput, "rb");
		if (vecFile==NULL)
		{
			CkPrintf("FATAL: Vector input file %s could not be read. Terminating.\n", _vectorInput);
			CkExit();
		}

		int N_vec, res;
		res = fread(&N_vec, sizeof(int), 1, vecFile);

		if (res != 1)
		{
			CkPrintf("FATAL: could not read size of input vector.\n");
			CkExit();
		}
		if (N_vec > _N) CkPrintf("Warning: input vector of size %d (expected %d).\n", N_vec, _N);
		else if (N_vec < _N)
		{
			CkPrintf("FATAL: input vector too small! (%d instead of %d entries)\n", N_vec, _N);
			CkExit();
		}

		for (i = 0; i < _N; i++)
		{
			res = fread(&x[i], sizeof(double), 1, vecFile);
			if (res != 1)
			{
				CkPrintf("Warning: vector file possibly corrupt. Read %d instead of %d entries.\n", i, N_vec);
				break;
			}
			if (fabs(x[i]) > 1.e-15) readNonNull = true;
		}

		fclose(vecFile);
		CkPrintf("Using vector '%s' (size %d).\n", _vectorInput, N_vec);
		if (!readNonNull) CkPrintf("!! WARNING: Vector consists only of zero-entries! Is it a text file?\n");
	}

	/*
	 * Validation of data
	 */


	/*
	 * Sequential test
	 */
	_ySeq = new double[_N];						// allocate result array
	for (i = 0; i < _N; i++) _ySeq[i] = 0.;		// null result array

	double tSeqStart = CkWallTimer();
	for (i = 0; i < _N; i++)
	{
		for (j = row_start[i]; j < row_start[i + 1]; j++)
		{
			_ySeq[i] += vals[j] * x[col_idx[j]];
		}
	}
	CkPrintf("Time for sequential calculation: %f\n", CkWallTimer() - tSeqStart);

	// if validate is true, save sequential results as a reference.
	if (!(_sliceMode & VALIDATE))
		delete[] _ySeq;


	// allocate and null other result array(s)
	_yNoSlice = NULL;
	_yRowSlice = NULL;
	_yMultiRowSlice = NULL;

	if (_sliceMode & NO_SLICE)
		_yNoSlice = new double[_N];
	if (_sliceMode & ROW_SLICE)
		_yRowSlice = new double[_N];
	if (_sliceMode & MULTI_SLICE)
		_yMultiRowSlice = new double[_N];

	this->nullResultArrays();


	/*
	 * General initializiation
	 */
	::mainProxy = thisProxy;
	_curStage = STAGE_INIT;
	_totalChares = 0;
	_charesFinished = 0;
	_runsFinished = 0;

	int numSlicesRow = 1;
	int numSlicesMult = 1;
	
	/*
	 * Determine number of chares and final slice size.
	 */
	if (_sliceMode & VALIDATE)
	{
		numSlicesRow = ceil(1. * rowMaxNnz / sliceSizeRow);
		numSlicesMult = ceil(1. * _nnz / sliceSizeMult);
		_totalChares = _N + _N*numSlicesRow + numSlicesMult;
	}
	else
	{
		if (_sliceMode == NO_SLICE)
		{
			_totalChares = _N;
		}
		else if (_sliceMode == ROW_SLICE)
		{
			numSlicesRow = ceil(1. * rowMaxNnz / sliceSizeRow);
			_totalChares = _N*numSlicesRow;
		}
		else if (_sliceMode == MULTI_SLICE)
		{
			numSlicesMult = ceil(1. * _nnz / sliceSizeMult);
			_totalChares = numSlicesMult;
		}
	}

	/*
	 * Generate chare arrays
	 */
	double tGenStart = CkWallTimer();
	if (_sliceMode & ROW_SLICE && (numSlicesRow > 1 || _sliceMode & VALIDATE))
	{
		CkPrintf("SINGLE ROW SLICING: Creating %d chares (%d rows with %d slices each of size %d).\n",
			_N*numSlicesRow, _N, numSlicesRow, sliceSizeRow);
		_sliceArray = CProxy_RowSlice::ckNew(_N, numSlicesRow);	// init 2D array of row chares
	}
	if (_sliceMode & MULTI_SLICE)
	{
		CkPrintf(" MULTI ROW SLICING: Creating %d chares (%d slices each with up to %d entries).\n",
			numSlicesMult, numSlicesMult, sliceSizeMult);
		_multiRowSliceArray = CProxy_MultiRowSlice::ckNew(numSlicesMult);
	}
	if (_sliceMode & NO_SLICE)
	{
		CkPrintf("        NO SLICING: Creating %d chares (%d rows).\n", _N, _N);
		_rowArray = CProxy_Row::ckNew(_N);
	}

	CkPrintf("Time for chare creation: %fs. Initializing chares..\n", CkWallTimer()-tGenStart);

	/*
	 * Distribute data to chares.
	 * Chares will report back to stageFinished() after initialization.
	 */
	if (_sliceMode & MULTI_SLICE)
	{
		/*
		 * This  piece of code needs some GOOD explanation.
		 * Took two hours to figure the row_start-stuff out.
		 */
		int curRow = 0;
		int curVal = 0;
		int rowStartIdx = 0;

		// try to distribute nonzero entries to the slices homogeneously,
		// i.e. letting the smallest chare(s) having only one entry less than
		// fully populated ones.
		// extraCapacity is thus the number of slices with (sliceSize-1) entries.
		int extraCapacity = sliceSizeMult * numSlicesMult - _nnz;
		CkAssert(extraCapacity >= 0 && extraCapacity < numSlicesMult);

		for (i = 0; i < numSlicesMult; i++)
		{
			int firstRow = curRow;
			int firstVal = curVal;

			// determine number of entries for this chare
			int thisNnz;
			if (i >= numSlicesMult - extraCapacity)
				thisNnz = sliceSizeMult - 1;
			else thisNnz = sliceSizeMult;

			if (thisNnz>_nnz) thisNnz = _nnz;


			// determine number of row breaks for this chare
			// (row break right before at the first element doesn't count toward this
			//  as it doesn't increase the chare's number of rows. It increases
			//  first and current row, however.)
			int nRows = 1;
			if (row_start[curRow + 1] == firstVal)
			{
				++firstRow;
				++curRow;
			}

			for (curVal = firstVal + 1; curVal < firstVal + thisNnz; curVal++)
			{
				if (row_start[curRow + 1] == curVal) // row break
				{
					++curRow;
					++nRows;
				}
			}

			_multiRowSliceArray[i].init(nRows, thisNnz, firstRow, firstVal, &vals[firstVal], &col_idx[firstVal],
				&row_start[firstRow + 1]);

			curVal = firstVal + thisNnz;
		}
	}
	if (_sliceMode & NO_SLICE || _sliceMode & ROW_SLICE)
	{
		for (i = 0; i < _N; i++)
		{
			int rowNnz = row_start[i + 1] - row_start[i]; // total number of nonzeroes in row
			int firstColIdx = row_start[i];				  // index (for val/col_idx arrays) of first nonzero entry

			if (_sliceMode & NO_SLICE)
			{
				_rowArray[i].init(i, rowNnz, &vals[firstColIdx], &col_idx[firstColIdx]);
			}
			if (_sliceMode & ROW_SLICE)
			{
				int nnzLeft = rowNnz;
				int startCol = firstColIdx;
				j = 0;

				// init chunks of size sliceSize
				while (nnzLeft >= sliceSizeRow)
				{
					_sliceArray(i, j).init(i, sliceSizeRow, &vals[startCol], &col_idx[startCol]);

					nnzLeft -= sliceSizeRow;
					++j;
					startCol = j*sliceSizeRow + firstColIdx;
				}
				// init the rest
				if (nnzLeft > 0)
				{
					_sliceArray(i, j).init(i, nnzLeft, &vals[startCol], &col_idx[startCol]);
					++j; // important, to not overwrite this chare down below!
				}
				// init blank unused chares
				for (j; j < numSlicesRow; j++)
				{
					_sliceArray(i, j).initBlank(i);
				}
			}
		}
	}
	
	// cleanup
	delete[] vals;
	delete[] col_idx;
	delete[] row_start;
}

// constructor for migration
Main::Main(CkMigrateMessage* msg)
{

}

Main::~Main()
{
	if (_ySeq != NULL) delete[] _ySeq;
	if (_yNoSlice != NULL) delete[] _yNoSlice;
	if (_yRowSlice != NULL) delete[] _yRowSlice;
	if (_yMultiRowSlice != NULL) delete[] _yMultiRowSlice;
}

void Main::finalize()
{
	int i;
	double tTotal = CkWallTimer() - _tStart;
	int flops_per_entry = 2;
	if (_runs > 1 && !(_sliceMode&VALIDATE))
	{
		CkPrintf("Finished %d runs after %.2f seconds (%f seconds per run)\n", _runs, tTotal, tTotal / _runs);
		CkPrintf("%.2f MFlops (%.3E operations, %d per entry)\n",
			1.*flops_per_entry*_runs*_nnz / tTotal / 1.e6, 1.*flops_per_entry*_runs*_nnz, flops_per_entry);
	}
	else CkPrintf("Finished after %f seconds.\n", tTotal);
	if (_N <= 20)
	{
		CkPrintf("Result  SEQUENTIAL:");
		for (i = 0; i < _N; i++)
			CkPrintf(" %f", _ySeq[i]);
		CkPrintf("\n");

		if (_sliceMode & NO_SLICE)
		{
			CkPrintf("Result    NO_SLICE:");
			for (i = 0; i < _N; i++)
				CkPrintf(" %f", _yNoSlice[i]);
			CkPrintf("\n");
		}
		if (_sliceMode & ROW_SLICE)
		{
			CkPrintf("Result   ROW_SLICE:");
			for (i = 0; i < _N; i++)
				CkPrintf(" %f", _yRowSlice[i]);
			CkPrintf("\n");
		}
		if (_sliceMode & MULTI_SLICE)
		{
			CkPrintf("Result MULTI_SLICE:");
			for (i = 0; i < _N; i++)
				CkPrintf(" %f", _yMultiRowSlice[i]);
			CkPrintf("\n");
		}
	}
	else CkPrintf("Result vector too long to display.\n");
	CkExit();
}

void Main::nextStage()
{
	int i;

	++_curStage;
	_charesFinished = 0;
	switch (_curStage)
	{
	case(STAGE_INIT):
		CkPrintf("Init should be already done!\n");
		CkAssert(false);
		CkExit();
		break;
	case(STAGE_CALCULATE):
		CkPrintf("Starting calculation - %d run(s)\n", _runs);
		_tStart = CkWallTimer();
		if (_sliceMode & ROW_SLICE) _sliceArray.calc();
		if (_sliceMode & NO_SLICE) _rowArray.calc();
		if (_sliceMode & MULTI_SLICE) _multiRowSliceArray.calc();
		break;
	case(STAGE_DONE) :
		++_runsFinished;
		if (_runsFinished < _runs)
		{
			if (1 == _runsFinished)
				CkPrintf("Finished 1st run. Estimated total time: %.2f seconds.\n", (CkWallTimer() - _tStart)*_runs);

			if (_sliceMode & VALIDATE) this->compareResults();

			// do another run
			--_curStage;
			_charesFinished = 0;
			this->nullResultArrays();

			if (_sliceMode & ROW_SLICE) _sliceArray.calc();
			if (_sliceMode & NO_SLICE) _rowArray.calc();
			if (_sliceMode & MULTI_SLICE) _multiRowSliceArray.calc();
		}
		else this->nextStage();
		break;
	case(STAGE_QUIT):
	default:
		CkPrintf("Calculation finished.\n");
		if (_sliceMode & VALIDATE ) this->compareResults(true);
		this->finalize();
		break;
	}
}

void Main::nullResultArrays()
{
	int i;

	if (_sliceMode & NO_SLICE)
		for (i = 0; i < _N; i++)
			_yNoSlice[i] = 0.;
	if (_sliceMode & ROW_SLICE)
		for (i = 0; i < _N; i++)
			_yRowSlice[i] = 0.;
	if (_sliceMode & MULTI_SLICE)
		for (i = 0; i < _N; i++)
			_yMultiRowSlice[i] = 0.;
}

void Main::compareResults(bool forceOutput)
{
	int i;
	int errors;
	double delta, res;
	bool valid = true;
	double EPS = 1.e-10;

	errors = 0; delta = 0.;
	for (i = 0; i < _N; i++)
		if ((res=fabs((_yNoSlice[i] - _ySeq[i]) / _ySeq[i])) > EPS)
		{
			++errors; valid = false;
			if (res > delta) delta = res;
		}
	if (errors > 0)
		CkPrintf("VALIDATION ERROR: NO_SLICE has %d (%.2f%%) components with error up to %E.\n",
		errors, 100.*errors / _N, delta);

	errors = 0; delta = 0.;
	for (i = 0; i < _N; i++)
		if ((res=fabs((_yRowSlice[i] - _ySeq[i]) / _ySeq[i])) > EPS)
		{
			++errors; valid = false;
			if (res > delta) delta = res;
		}
	if (errors > 0)
		CkPrintf("VALIDATION ERROR: ROW_SLICE has %d (%.2f%%) components with error up to %E.\n",
		errors, 100.*errors / _N, delta);

	errors = 0; delta = 0.;
	for (i = 0; i < _N; i++)
		if ((res=fabs((_yMultiRowSlice[i] - _ySeq[i]) / _ySeq[i])) > EPS)
		{
			++errors; valid = false;
			if (res > delta) delta = res;
		}
	if (errors > 0)
		CkPrintf("VALIDATION ERROR: MULTI_SLICE has %d (%.2f%%) components with error up to %E.\n",
		errors, 100.*errors / _N, delta);

	if (forceOutput && valid)
	{
		CkPrintf("VALIDATION SUCCESS.\n"
			"Same results for Sequential, No Slicing, Row Slicing and Multi Row Slicing calculations.\n");
	}
}

void Main::setResultNoSlice(int row, double res)
{
	_yNoSlice[row] += res;
	this->stageFinished();
}

void Main::setResultRowSlice(int row, double res)
{
	_yRowSlice[row] += res;
	this->stageFinished();
}

void Main::setResultMultiRowSlice(int nRows, int firstRow, double *res)
{
	for (int i = 0; i < nRows; i++)
	{
		_yMultiRowSlice[i + firstRow] += res[i];
	}
	this->stageFinished();
}

void Main::stageFinished()
{
	++_charesFinished;
#ifdef VERBOSE
	if (_charesFinished == _totalChares || _charesFinished % int(_totalChares / 10) == 0)
		CkPrintf("%d/%d finished (%.1f%%)\n", _charesFinished, _totalChares, 100.*_charesFinished / _totalChares);
#endif
	if (_charesFinished == _totalChares)
		this->nextStage();
}

#include "main.def.h"
