CC=gcc
CFLAGS=-O2 -ggdb

elf_patcher: main.c setup.o setup.h
	${CC} ${CFLAGS} -o elf_patcher main.c setup.o setup.h

clean:
	rm elf_patcher setup.o setup.h.gch

setup.o: setup.c setup.h
	${CC} ${CFLAGS} -c setup.c setup.h
