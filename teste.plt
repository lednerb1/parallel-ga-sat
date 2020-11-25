#!/usr/bin/gnuplot
reset

set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'teste.png'

set border linewidth 1.5

set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7

unset key

set xlabel 'Speedup?'
set ylabel 'Threads?'
set tics scale 0.75

set xrange[]


plot 'inputs/3sat-003-002.cnf.dat' using 1:2 with yerrorbars linestyle 1