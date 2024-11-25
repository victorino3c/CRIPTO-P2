set terminal pngcairo size 800,600
set output "data/avalanche_des_rounds.png"
set title "Efecto Avalancha por Ronda - Clave inicial varias iteraciones"
set xlabel "Ronda"
set ylabel "Número de Bits Cambiados"
set grid
plot "data/changes.txt" using 1 with linespoints title "Bits cambiados"
