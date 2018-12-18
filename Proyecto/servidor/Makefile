# Makefile para el servidor
# Autor: Diego Jesus Favela Nava & Ricardo Martínez Ríos
# Fecha: 15/Dec/2018

CC=gcc
CFLAGS= -Wall

all:	*.c
	$(CC) *.c -o szserver $(FLAGS) -lpthread

doc:
	doxygen doc_servidor

clean:
	rm -f szserver
	rm -rf html latex man
