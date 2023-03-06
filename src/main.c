#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <elf.h>

#include "setup.h"


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


int main(int argc, char ** argv) {

	FILE * target_file;
	Elf64_Ehdr elf_header;

	libc_func * func_table; //gets allocated by get_func_table
	char replace_table[FUNC_REPL_NUM][FUNC_NAME_MAX] = {};

	//check arguments
	if (argc < 2) {
		printf("usage: %s <file>\n", argv[0]);
		return 1;
	}

	//try to open elf target
	target_file = fopen(argv[1], "r+");
	if (target_file == NULL) {
		perror("target open");
		return 1;
	}

	//get headers
	read_elf_header(&elf_header, target_file);

	//check headers
	check_magic_bytes(&elf_header);

	//TODO test
	get_func_table(&elf_header, target_file, func_table, replace_table);

	fclose(target_file);
}
