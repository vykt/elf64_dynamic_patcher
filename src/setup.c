#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <unistd.h>

#include "setup.h"


//read arbitrary section
size_t read_section_header(Elf64_Shdr * section_header, FILE * target_file) {

	size_t ret;
	ret = fread(section_header, sizeof(Elf64_Shdr), 1, target_file);
	
	if (ferror(target_file)) {
		puts("Section header: read failed with error.");
		clearerr(target_file);
		return 1;
	}

	return 0;
}

//read section string table, used to resolve future ambiguous sections
size_t read_shstrtab_header(Elf64_Ehdr * elf_header, Elf64_Shdr * shstrtab_header, FILE * target_file) {

	size_t ret;
	char * string_buffer;
	string_buffer = malloc(FUNC_NAME_MAX);
	
	for (int i = 0; i < elf_header->e_shnum; ++i) {

		fseek(target_file, elf_header->e_shoff + i*sizeof(Elf64_Shdr), SEEK_SET);
		if (read_section_header(shstrtab_header, target_file)) continue;

		if (shstrtab_header->sh_type == SHT_STRTAB) {

			fseek(target_file, shstrtab_header->sh_offset + shstrtab_header->sh_name, SEEK_SET);
			ret = fread(string_buffer, FUNC_NAME_MAX, 1, target_file);
			//If found .shstrtab section
			if (!strncmp(string_buffer, ".shstrtab", 10)) {
				return 0;
			}
		}
	} //end for

	puts("ELF header: failed to find section string table.");
	return 1;
}


//read dynsym section header
void get_func_table(Elf64_Ehdr * elf_header, FILE * target_file, libc_func * func_table, char replace_table[FUNC_REPL_NUM][FUNC_NAME_MAX]) {

	size_t ret;
	char * string_buffer;
	int func_count = 0;
	int section_count = 0;

	Elf64_Shdr temp_header;

	Elf64_Shdr shstrtab_header;
	Elf64_Shdr dynsym_header;
	Elf64_Shdr dynstr_header;
	Elf64_Shdr strtab_header;
	Elf64_Shdr plt_header;

	Elf64_Sym symbol;


	string_buffer = malloc(FUNC_NAME_MAX); //TODO remember to free

	//first get section string table
	if (read_shstrtab_header(elf_header, &shstrtab_header, target_file)) exit(1);


	//now fill every other section header
	for (int i = 0; i < elf_header->e_shnum; ++i) {
		
		//seek to i'th section header
		fseek(target_file, elf_header->e_shoff + i*sizeof(Elf64_Shdr), SEEK_SET);
		
		//if section header read was successful
		if (read_section_header(&temp_header, target_file)) exit(1);	
		


		//potential .plt found
		if (temp_header.sh_type == SHT_PROGBITS) {

			fseek(target_file, shstrtab_header.sh_offset + temp_header.sh_name, SEEK_SET);
			ret = fread(string_buffer, FUNC_NAME_MAX, 1, target_file);

			printf("%s\n", string_buffer);

			//match found for .plt
			if (!strncmp(string_buffer, ".plt", 5)) {
				memcpy(&plt_header, &temp_header, sizeof(Elf64_Shdr));
			}

		//potential .rela.plt found
		//TODO, check HOWTO

		//if dynsym found
		} else if (temp_header.sh_type == SHT_DYNSYM) {
			memcpy(&dynsym_header, &temp_header, sizeof(Elf64_Shdr));

			//seek to relevant string section
			fseek(target_file, elf_header->e_shoff + dynsym_header.sh_link*sizeof(Elf64_Shdr), SEEK_SET);

			//read the corresponding strings section
			if (read_section_header(&dynstr_header, target_file)) {
				puts("Strtab header: unable to find corresponding string table.");
				exit(1);
			}

			//for every symbol in the header
			for (int j = 0; j < dynsym_header.sh_size / sizeof(Elf64_Sym); ++j) {
				
				//seek to specific symbol in .dynsym
				fseek(target_file, dynsym_header.sh_offset + j*sizeof(Elf64_Sym), SEEK_SET);
				//read the symbol from .dynsym
				ret = fread(&symbol, sizeof(Elf64_Sym), 1, target_file);

				//seek to specific symbol in corresponding string section
				fseek(target_file, dynstr_header.sh_offset + symbol.st_name, SEEK_SET);
				ret = fread(string_buffer, FUNC_NAME_MAX, 1, target_file);
				printf("Symbol %d: %s\n", j, string_buffer);
			}

		} //end if dynsym

	} //end for
}
