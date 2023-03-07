CC=gcc
CFLAGS=-O2 -ggdb

elf_patcher: main.c setup.o setup.h vector.o vector.h
	${CC} ${CFLAGS} -o elf_patcher main.c setup.o setup.h vector.o vector.h

clean:
	rm elf_patcher setup.o setup.h.gch vector.o vector.h.gch

setup.o: setup.c setup.h vector.h
	${CC} ${CFLAGS} -c setup.c setup.h vector.h

vector.o: vector.c vector.h
	${CC} ${CFLAGS} -c vector.c vector.h
