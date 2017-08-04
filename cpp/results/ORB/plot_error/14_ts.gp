set term postscript eps enhanced color
set output "14_ts.eps"
set size ratio 0.5
set yrange [0:*]
set xlabel "Speed [km/h]"
set ylabel "Translation Error [%]"
plot "14_ts.txt" using ($1*3.6):($2*100) title 'Translation Error' lc rgb "#0000FF" pt 4 w linespoints
