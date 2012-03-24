#!/bin/bash

gcc fcfs.c -o fcfs
#./fcfs /home/smit/kaushik/Applegate/prob/la35.prb
./fcfs car3.prb
#./bottle -n gen -s /home/smit/kaushik/Applegate/prob/la35.prb

#python gantt.py -o bottle.gpl -t bottle bottle.txt
#python gantt.py -o bottle_mach.gpl -t bottle_mach bottle_mach.txt
python gantt.py -o fcfs_mach.gpl -t fcfs_mach fcfs_mach.txt

python gantt.py -o fcfs.gpl -t fcfs fcfs.txt

#gnuplot bottle.gpl
#gnuplot bottle_mach.gpl
gnuplot fcfs_mach.gpl
gnuplot fcfs.gpl
