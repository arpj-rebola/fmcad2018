set key left
set xlabel "solved instances"
set ylabel "time (seconds)"
plot 'data.txt' using 1:2 with linespoints pointtype 1 title 'drat-trim',\
'data.txt' using 1:3 with linespoints pointtype 2 title 'rupee-operational',\
'data.txt' using 1:4 with linespoints pointtype 3 title 'rupee-specified'
#plot 'data.txt' with lines using 1:3
#plot 'data.txt' with lines using 1:4

