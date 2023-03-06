#ifndef setup_h
#define setup_h

#include <stdio.h>
#include <elf.h>


#define FUNC_REPL_NUM 2
#define FUNC_NAME_MAX 1024


/*
 *	TODO look at rel.plt or rela.plt and see how .plt and .symtab relate
 */
typedef struct {

	char name[FUNC_NAME_MAX];
	uint64_t offset;

} libc_func;


void get_func_table(Elf64_Ehdr * elf_header, FILE * target_elf, libc_func * func_table, char replace_table[FUNC_REPL_NUM][FUNC_NAME_MAX]);

#endif
