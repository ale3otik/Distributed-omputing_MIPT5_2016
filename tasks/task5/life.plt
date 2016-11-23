#set terminal wxt

set terminal emf monochrome size 800, 800
set output  'life.emf'

set style line 1 linecolor 0 pointtype 5 pointsize 0.8

set xrange [0.0:100]
set yrange [0.0:100] 

set xlabel "x"
set ylabel "y"

plot 'life.dat' using 1:2 w points ls 1 notitle

unset output         