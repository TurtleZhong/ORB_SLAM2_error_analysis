set term postscript eps enhanced color
set output "18.eps"
set size ratio -1
set xrange [-206:23]
set yrange [-99:131]
set xlabel "x [m]"
set ylabel "z [m]"
plot "18.txt" using 1:2 lc rgb "#FF0000" title 'Ground Truth' w lines,"18.txt" using 3:4 lc rgb "#0000FF" title 'Visual Odometry' w lines,"< head -1 18.txt" using 1:2 lc rgb "#000000" pt 4 ps 1 lw 2 title 'Sequence Start' w points
