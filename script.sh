#!/bin/bash

gcc fcfs-single.c -o fcfs-single
#./fcfs /home/smit/kaushik/Applegate/prob/la35.prb
./fcfs-single /home/smit/kaushik/Applegate/prob/la35.prb
./bottle -n gen -s /home/smit/kaushik/Applegate/prob/la35.prb

python gantt.py -o bottle.gpl -t bottle bottle.txt
python gantt.py -o bottle_mach.gpl -t bottle_mach bottle_mach.txt
python gantt.py -o fcfs_mach.gpl -t fcfs_mach fcfs_mach.txt
python gantt.py -o fcfs.gpl -t fcfs fcfs.txt

gnuplot bottle.gpl
gnuplot bottle_mach.gpl
gnuplot fcfs_mach.gpl
gnuplot fcfs.gpl
