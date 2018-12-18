# Makefile para el cliente
# Autor: Diego Jesus Favela Nava & Ricardo Martínez Ríos
# Fecha: 15/Dec/2018

CC=gcc
CFLAGS= -Wall

all:	*.c
	$(CC) *.c -o szclient $(FLAGS)

doc:
	doxygen doc_cliente

clean:
	rm -f szclient box/*.png
	rm -rf html latex man
