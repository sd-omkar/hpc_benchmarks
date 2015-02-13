#!/usr/bin/python
import os

inFiles= [ 	"rnd1000_50000", \
			"rnd10000_1000000", \
			"rnd30000_20000000",\
			"rnd30000_50000000" ]
			
runs = {	"rnd1000_50000" : 20000, \
			"rnd10000_1000000" : 2000, \
			"rnd30000_20000000" : 200, \
			"rnd30000_50000000" : 200 }

for ncores in range(1,16):
	for f in inFiles:
		os.system("./charmrun +p%d ./spmv -I %s.csr -n %d > bench/%s.log" % (ncores, f, runs[f], str(runs[f])+"runs_of_"+f+"_"+str(ncores)+"core"))
		print "%d runs of %s finished (%d cores)." % (runs[f], f, ncores)