scale=ARG1

set title "Performance"
set xlabel "Data amount (KB)"
set ylabel "time (ms)"
set terminal png font " Times_New_Roman,12 "
set output "perf.png"
set xtics 1,scale
set key left 

plot \
"data.csv" using 1:2 with linespoints linewidth 2 title "Semaphore", \
"data.csv" using 1:3 with linespoints linewidth 2 title "Lock-free"  \
