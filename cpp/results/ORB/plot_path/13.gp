set term postscript eps enhanced color
set output "13.eps"
set size ratio -1
set xrange [-221:818]
set yrange [-72:967]
set xlabel "x [m]"
set ylabel "z [m]"
plot "13.txt" using 1:2 lc rgb "#FF0000" title 'Ground Truth' w lines,"13.txt" using 3:4 lc rgb "#0000FF" title 'Visual Odometry' w lines,"< head -1 13.txt" using 1:2 lc rgb "#000000" pt 4 ps 1 lw 2 title 'Sequence Start' w points
