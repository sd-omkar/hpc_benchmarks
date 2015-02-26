#!/usr/bin/python
import os

inFiles= os.listdir("/home/idetc-heller/spmv_charm/ang")
			
runs = 500
count = 0
			
for f in inFiles:
	cmd = "./charmrun +p8 ./spmv -I %s -n %d -m 2048 > bench/%s.log" % ("/home/idetc-heller/spmv_charm/ang/"+f, runs, f)
	print cmd
	os.system(cmd)
	count += 1
	print "%s finished - %d/%d done." % (f, count, len(inFiles))