#!/usr/bin/python
import os

inFiles= [ 	"rnd1000_50000", \
			"rnd10000_1000000", \
			"rnd30000_20000000",\
			"rnd30000_50000000" ]
			
runs = {	"rnd1000_50000" : 100000, \
			"rnd10000_1000000" : 10000, \
			"rnd30000_20000000" : 1000, \
			"rnd30000_50000000" : 1000 }

for ncores in range(1,16+1):
	for f in inFiles:
		cmd = "./charmrun +p%d ./spmv -I %s.csr -n %d -s 256 -S 16 > bench/%s.log" % (ncores, f, runs[f], str(runs[f])+"runs_of_"+f+"_"+str(ncores)+"core")
		print cmd
		os.system(cmd)
		print "%d runs of %s finished (%d cores)." % (runs[f], f, ncores)