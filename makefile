CC = gcc 
FLAGS = -c -Wall -ansi -pedantic -std=c99
LIBRARY = -lgmp
P=seguridadPerfecta/
O=obj/
A=afin/
U=utiles/
T=textos/

all: $(P)seguridad_perfecta

run_seguridad_perfecta_P: $(P)seguridad_perfecta
	$(P)seguridad_perfecta -P -i $(T)hamlet.txt

run_seguridad_perfecta_I: $(P)seguridad_perfecta
	$(P)seguridad_perfecta -I -i $(T)hola.txt

# COMANDOS EJECUCION AFIN #
run_afin_D: $(A)afin
	$(A)afin -D -m 26 -a 3 -b 7 -i $(T)adios.txt -o $(T)adios.txt

run_afin_C: $(A)afin
	$(A)afin -C -m 26 -a 3 -b 7 -i $(T)hola.txt -o $(T)adios.txt

###############################################################################
#EJECUTABLES                                                                  #
###############################################################################

$(P)seguridad_perfecta: $(O)seguridad_perfecta.o $(O)afin.o $(O)utils.o
	$(CC) -o $@ $^ $(LIBRARY)

$(O)seguridad_perfecta.o: $(P)seguridad_perfecta.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<


$(O)afin.o: $(A)afin.c $(A)afin.h $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

$(O)utils.o: $(U)utils.c $(U)utils.h
	$(CC) -o $@ $(FLAGS) $<

clean:
	rm -f $(O)*.o $(P)seguridad_perfecta 