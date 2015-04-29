#!/bin/bash
#PBS -l nodes=1:ppn=16:intel,walltime=12:00:00:00

cd spmv_charm
python bench.py 8 > bench8.log
python parselog.py bench8 > bench8.csv
