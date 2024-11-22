CC = gcc 
FLAGS = -c -Wall -ansi -pedantic -std=c99
LIBRARY = -lgmp
P=seguridadPerfecta/
O=obj/
A=afin/
U=utiles/
T=textos/
D=des/
AS=aes/

all: $(P)seg-perf $(D)des $(AS)aes_no_lineal

run_seg-perf_P: $(P)seg-perf
	$(P)seg-perf -P -i $(T)hamlet.txt -o $(P)salida.txt

run_seg-perf_I: $(P)seg-perf
	$(P)seg-perf -I -i $(T)hamlet.txt -o $(P)salida.txt

run_des: $(D)des
	$(D)des -C -k 1234567890123456 -i $(T)hola.txt -o $(T)adios.txt

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


$(A)afin: $(O)afin.o
	$(CC) -o $@ $^ $(LIBRARY)

$(D)des: $(O)des.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)des.o: $(D)des.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(AS)aes_no_lineal: $(O)aes.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)aes.o: $(AS)noLinealidadS-box.c
	$(CC) -o $@ $(FLAGS) $<

$(O)afin.o: $(A)afin.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)utils.o: $(U)utils.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

clean:
	rm -f $(O)*.o $(P)seg-perf $(D)des $(AS)aes $(A)afin