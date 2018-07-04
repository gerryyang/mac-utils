# order_center

set title "Performance of order-center from Portal"

set label 1 "cgi: insert 1\n\
order: select 2, insert 1, update 4\n\
provide: select 2, update 4"
set label 1 at graph 0.02, 0.85 tc lt 2

#set terminal jpeg medium
#set output "tps-order-center.jpeg"

set boxwidth 0.3 absolute
set style fill solid 1.000000 border -1
set nokey

set xtics rotate by -45

set ylabel "TPS"
set xlabel "The length of PortalExtendField"

set yrange [0:1000]

plot 'order_center-portal.dat' using 2:xticlabels(1) with boxes

#replot
#set output


