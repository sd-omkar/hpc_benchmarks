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
dir = "bench/"






#
# Only change stuff below this line if you really want to alter the script.
#
files = os.listdir(dir)

lookFor = {"flops":"MFlops", "dimensions":"max nonzeroes per row"}
outputLines = {}

print '"sep= "'
print "FILE ROWS NNZ MAX_NNZ_PER_ROW MFLOPS"

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
			
			print fn, rows, nnz, max_row_nnz, mflops
		
		except:
			print fn, "FAILURE"
			continue