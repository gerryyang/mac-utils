# order_center

reset
set title "Performance of order-center"

#set terminal jpeg medium
#set output "tps-order-center.jpeg"

set boxwidth 0.3 absolute
set style fill solid 1.000000 border -1
set nokey

set xtics rotate by -45

set ylabel "TPS"
set xlabel "Mode Options"

set yrange [0:10000]

plot 'order_center.dat' using 2:xticlabels(1) with boxes

#replot
#set output


