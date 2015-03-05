#!/bin/bash
#PBS -l nodes=1:ppn=32:amd,walltime=12:00:00:00

cd spmv_charm
python bench32.py > bench32.log
python parselog.py > bench32.csv
mv bench bench32
mkdir bench
