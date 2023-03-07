#ifndef SETUP_H
#define SETUP_H

#include <stdio.h>
#include <elf.h>

#include "vector.h"

#define FUNC_REPL_NUM 2
#define FUNC_NAME_MAX 1024


/*
 *	TODO look at rel.plt or rela.plt and see how .plt and .symtab relate
 */
typedef struct {

	char name[FUNC_NAME_MAX];
	uint64_t offset;

} libc_func_t;


void get_func_vector(Elf64_Ehdr * elf_header, FILE * target_elf, vector_t * func_vector);

#endif
