CC = gcc 
FLAGS = -c -Wall -ansi -pedantic -std=c99
LIBRARY = -lgmp -ljpeg
P=seguridadPerfecta/
O=obj/
A=afin/
U=utiles/
T=textos/
B=binarios/
D=des/
AS=aes/

all: $(P)seg-perf $(D)des_cypher $(AS)aes_no_lineal

run_seg-perf_P: $(P)seg-perf
	$(P)seg-perf -P -i $(T)hamlet.txt -o $(P)salida.txt

run_seg-perf_I: $(P)seg-perf
	$(P)seg-perf -I -i $(T)hamlet.txt -o $(P)salida.txt

run_des_c: $(D)des_cypher
	$(D)des_cypher -C -k 12345F789AB23456 -i $(B)memoria-p1.pdf -o $(B)cyphered_memoria-p1.pdf

run_des_image_c: $(D)des_cypher
	$(D)des_cypher -C -k 12345F789AB23456 -i $(B)background.jpg -o $(B)cyphered_background.jpg

run_des_d: $(D)des_cypher
	$(D)des_cypher -D -k 12345F789AB23456 -i $(B)cyphered_memoria-p1.pdf -o $(B)cyphered_memoria-p1.pdf

run_des_image_d: $(D)des_cypher
	$(D)des_cypher -D -k 12345F789AB23456 -i $(B)cyphered_background.jpg -o $(B)cyphered_background.jpg

run_aes: $(AS)aes_no_lineal
	$(AS)aes_no_lineal -o

# COMANDOS EJECUCION AFIN #
run_afin_D: $(A)afin
	$(A)afin -D -m 26 -a 3 -b 7 -i $(T)adios.txt -o $(T)adios.txt

run_afin_C: $(A)afin
	$(A)afin -C -m 26 -a 3 -b 7 -i $(T)hola.txt -o $(T)adios.txt

###############################################################################
#EJECUTABLES                                                                  #
###############################################################################

$(P)seg-perf: $(O)seg-perf.o $(O)afin.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)seg-perf.o: $(P)seg-perf.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(D)des_cypher: $(O)des_cypher.o $(O)des.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)des_cypher.o: $(D)des_cypher.c $(D)des.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(A)afin: $(O)afin.o
	$(CC) -o $@ $^ $(LIBRARY)

$(D)des: $(O)des.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)des.o: $(D)des.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(AS)aes_no_lineal: $(O)aes_no_lineal.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)aes_no_lineal.o: $(AS)aes_no_lineal.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)afin.o: $(A)afin.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)utils.o: $(U)utils.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

clean:
	rm -f $(O)*.o $(P)seg-perf $(D)des_cypher $(AS)aes_no_lineal $(A)afin $(AS)histogram.png $(AS)frequencies.txt