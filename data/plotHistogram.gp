set terminal png size 1500,900
set output 'data/histogram.png'
set title 'Histogram of Different Bits with 1024 tests'
set xlabel 'Number of Different Bits'
set ylabel 'Frequency'
set style data histograms
set style fill solid 1.0 border -1
set boxwidth 0.9
set grid ytics
set ytics 5
set xtics 3
set yrange [0:*]  # Ajusta para que comience en 0 y se ajuste automáticamente al máximo.


# Ahora asignamos el índice de cada línea (1, 2, 3, ..., 128) como el eje X
plot 'data/frequencies.txt' using (column(0) + 1):1 with boxes title 'Frequency'
