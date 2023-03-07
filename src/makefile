CC=gcc
CFLAGS=-O2 -ggdb

elf_patcher: main.c patch.o patch.h setup.o setup.h vector.o vector.h
	${CC} ${CFLAGS} -o elf_patcher main.c patch.o patch.h setup.o setup.h vector.o vector.h

clean:
	rm elf_patcher patch.o patch.h.gch setup.o setup.h.gch vector.o vector.h.gch

patch.o: patch.c patch.h setup.h vector.h
	${CC} ${CFLAGS} -c patch.c patch.h setup.h vector.h

setup.o: setup.c setup.h vector.h
	${CC} ${CFLAGS} -c setup.c setup.h vector.h

vector.o: vector.c vector.h
	${CC} ${CFLAGS} -c vector.c vector.h
