#!/bin/bash
python golsn.py $1.txt > $1cnf.txt 
./minisat2.exe $1cnf.txt $1cnfout.txt 
python golsnout.py $1cnfout.txt > $1out.txt 
python checkresults.py $1.txt $1cnfout.txt