#!/bin/bash

gcc fcfs-single.c -o fcfs-single.o
./fcfs-single.o /home/smit/kaushik/Applegate/prob/la30.prb
./bottle -n gen -s /home/smit/kaushik/Applegate/prob/la30.prb
 

python gantt.py -o bottle.gpl -t bottle bottle.txt
python gantt.py -o bottle_mach.gpl -t bottle_mach bottle_mach.txt
python gantt.py -o fcfs_mach.gpl -t fcfs_mach fcfs_mach.txt
python gantt.py -o fcfs.gpl -t fcfs fcfs.txt

gnuplot bottle.gpl
gnuplot bottle_mach.gpl
gnuplot fcfs_mach.gpl
gnuplot fcfs.gpl

cd answers
mkdir la31
cp ../statistics.txt ../fcfs.png ../bottle.png ../fcfs_mach.png ../bottle_mach.png ./la31/


