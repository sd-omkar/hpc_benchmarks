#!/bin/bash
#PBS -l nodes=1:ppn=16:intel,walltime=12:00:00:00

cd spmv_charm
python bench.py 20 30 > bench20.log
python parselog.py bench20 > bench20.csv
