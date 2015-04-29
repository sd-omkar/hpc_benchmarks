#!/usr/bin/python
import os


#
#
# Usage: 
#   Set 'dir' to whatever directory you want to parse for spmv logfiles (just the output of some 'spmv [args] > logfile' operation).
#   All files in that directory will be parsed for benchmarking results, and output in the form of
#     filename rows nnz maxNnzPerRow MFlops
#   will be written to stdout.
#
#   Just use 'parselog.py > myResults.csv' for easy viewing in Excel.
#
#
dir = "bench"
if len(sys.argv) > 1:
	dir = sys.argv[1]





#
# Only change stuff below this line if you really want to alter the script.
#
files = os.listdir(dir)

lookFor = {"flops":"MFlops", "dimensions":"max nonzeroes per row",
		   "slice":"slices each with up to",
		   "time":"seconds per run"}
outputLines = {}

print '"sep= "'
print "FILE ROWS NNZ MAX_NNZ_PER_ROW MFLOPS SLICE CHARES RUNS TIME"

for fn in files:
	with open(os.path.join(dir, fn), "r") as f:
		s = f.read()
		f.seek(0)
		lines = f.readlines()
		success = (s.find("MFlops") != -1)
		
		if not success:
			print fn, "FAILURE"
			continue
		
		# find lines where important information is written in output.
		for k in lookFor.keys():
			for line in lines:
				if line.find(lookFor[k]) != -1:
					outputLines[k] = line
					break
		
		try:	
			# extract MFlops
			# sample line: '2478.75 MFlops (5.719E+09 operations, 2 per entry)'
			mflops = float( outputLines["flops"].strip().split()[0] )
			
			# extract dimensions
			# sample line: '155331 rows, 5719417 nonzeroes, 2415 max nonzeroes per row.'
			#                 [0]   [1]     [2]      [3]     [4] [5]      [6]  [7] [8]
			dimWords = outputLines["dimensions"].strip().split()
			
			rows = int(dimWords[0])
			nnz = int(dimWords[2])
			max_row_nnz = int(dimWords[4])
			
			# extract number of chares, slice size
			# sample line: ' MULTI ROW SLICING: Creating 1889 chares (1889 slices each with up to 2048 entries).'
			#				   [0] [1]    [2]     [3]     [4]  [5]     [6]  [7]    [8] [9]         [12]  [13]
			sliceWords = outputLines["slice"].strip().split()
			
			chares = int(sliceWords[4])
			slice = int(sliceWords[12])
			
			# extract wall time for calculation
			# sample line: 'Finished 13750 runs after 80.34 seconds (0.005843 seconds per run)'
			#                  [0]     [1]  [2]   [3]  [4]   [5]    ...
			timeWords = outputLines["time"].strip().split()
			
			runs = int(timeWords[1])
			walltime = float(timeWords[4])
			
			
			print fn, rows, nnz, max_row_nnz, mflops, slice, chares, runs, walltime
		
		except:
			print fn, "FAILURE"
			continue