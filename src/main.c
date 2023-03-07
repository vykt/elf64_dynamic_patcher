#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <elf.h>

#include "patch.h"
#include "setup.h"
#include "vector.h"


//check whether target is a valid ELF
void check_magic_bytes(Elf64_Ehdr * elf_header) {

	if ((elf_header->e_ident[EI_MAG0] != ELFMAG0)
		|| (elf_header->e_ident[EI_MAG1] != ELFMAG1)
		|| (elf_header->e_ident[EI_MAG2] != ELFMAG2)
		|| (elf_header->e_ident[EI_MAG3] != ELFMAG3)) {

			puts("ELF magic bytes: invalid, is this an ELF file?");
			exit(1);
	}

}


//read target headers into struct
void read_elf_header(Elf64_Ehdr * elf_header, FILE * target_file) {

	size_t ret;
	ret = fread(elf_header, sizeof(Elf64_Ehdr), 1, target_file);
	
	if (ferror(target_file)) {
		puts("ELF headers: read failed with error.");
		exit(1);
	}
}


//build sub table
void build_sub_table(int argc, char ** argv, repl_t * sub_table, vector_t func_vector) {

	int ret;
	int match = 0;
	libc_func_t temp_func;

	for (int i = 0; i < func_vector.length; ++i) {

		ret = vector_get(&func_vector, i, (char *) &temp_func);
		if (!(strcmp(argv[2], temp_func.name))) {
			sub_table->offset_old = temp_func.offset;
			++match;
		}
		if (!(strcmp(argv[3], temp_func.name))) {
			sub_table->offset_new = temp_func.offset;
			++match;
		}
	}
	if (match != 2) {
		puts("Input: unable to find matching old and/or new function symbols");
		exit(1);
	}
}


//le main
int main(int argc, char ** argv) {

	int ret;
	FILE * target_file;
	Elf64_Ehdr elf_header;

	vector_t func_vector;
	repl_t sub_table;	

	//check arguments
	if (argc < 4) {
		printf("usage: %s <file> <old_func> <new_func>\n", argv[0]);
		return 1;
	}

	//try to open elf target
	target_file = fopen(argv[1], "r+");
	if (target_file == NULL) {
		perror("target open");
		return 1;
	}

	//init vector
	ret = vector_ini(&func_vector, sizeof(libc_func_t));

	//get headers
	read_elf_header(&elf_header, target_file);

	//check headers
	check_magic_bytes(&elf_header);

	//get function vector, with names and offsets
	get_func_vector(&elf_header, target_file, &func_vector);
	

	//build the substitution table
	build_sub_table(argc, argv, &sub_table, func_vector);

	printf("old: 0x%lx, new: 0x%lx\n", sub_table.offset_old, sub_table.offset_new);

	fclose(target_file);

}
