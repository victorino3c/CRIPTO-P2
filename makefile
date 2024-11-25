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
DA=data/

all: $(P)seg-perf $(D)des_cypher $(D)triple_des_cypher $(AS)aes_no_lineal $(AS)SBOX_AES 

run_seg-perf_P: $(P)seg-perf
	$(P)seg-perf -P -i $(T)hamlet.txt -o $(P)salida.txt

run_seg-perf_I: $(P)seg-perf
	$(P)seg-perf -I -i $(T)hamlet.txt -o $(P)salida.txt

run_des_c: $(D)des_cypher
	$(D)des_cypher -C -k 12345F789AB23456 -i $(B)memoria-p1.pdf -o $(B)cyphered_memoria-p1.pdf

run_des_image_c: $(D)des_cypher
	$(D)des_cypher -C -k 12345F789AB23456 -i $(B)background.jpg -o $(B)cyphered_background.jpg

run_des_d: $(D)des_cypher
	$(D)des_cypher -D -k 12345F789AB23456 -i $(B)cyphered_memoria-p1.pdf -o $(B)decyphered_memoria-p1.pdf

run_des_image_d: $(D)des_cypher
	$(D)des_cypher -D -k 12345F789AB23456 -i $(B)cyphered_background.jpg -o $(B)decyphered_background.jpg

run_triple_des_c: $(D)triple_des_cypher
	$(D)triple_des_cypher -C -k 133457799BBCDFF11223345566778899AABBCCDDEEFF0011 -i $(B)memoria-p1.pdf -o $(B)cyphered_memoria-p1.pdf

run_triple_des_d: $(D)triple_des_cypher
	$(D)triple_des_cypher -D -k 133457799BBCDFF11223345566778899AABBCCDDEEFF0011 -i $(B)cyphered_memoria-p1.pdf -o $(B)decyphered_memoria-p1.pdf

run_des_no_lineal: $(D)des_no_lineal
	$(D)des_no_lineal -g bar -n random -r 1000000

run_aes_no_lineal: $(AS)aes_no_lineal
	$(AS)aes_no_lineal -o

run_SBOX_AES: $(AS)SBOX_AES
	$(AS)SBOX_AES -D

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

$(D)triple_des_cypher: $(O)triple_des_cypher.o $(O)des.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)triple_des_cypher.o: $(D)triple_des_cypher.c $(D)des.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(D)des_no_lineal: $(O)des_no_lineal.o $(O)des.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)des_no_lineal.o: $(D)des_no_lineal.c $(D)des.h $(U)utils.h
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

$(AS)SBOX_AES: $(O)SBOX_AES.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)SBOX_AES.o: $(AS)SBOX_AES.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)afin.o: $(A)afin.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)utils.o: $(U)utils.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

clean:
	rm -f $(O)*.o $(P)seg-perf $(D)des_cypher $(D)triple_des_cypher $(D)des_no_lineal $(AS)aes_no_lineal $(AS)SBOX_AES $(A)afin $(DA)histogram.png $(DA)frequencies.txt