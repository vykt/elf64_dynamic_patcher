#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <unistd.h>

#include "setup.h"
#include "vector.h"

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
				free(string_buffer);
				return 0;
			}
		}
	} //end for

	puts("ELF header: failed to find section string table.");
	free(string_buffer);
	return 1;
}


size_t populate_func_vector(Elf64_Shdr * plt_header, Elf64_Shdr * relaplt_header, Elf64_Shdr * dynsym_header, Elf64_Shdr * dynstr_header, vector_t * func_vector, FILE * target_file) {

	size_t ret;
	int vector_ret;
	uint32_t temp_info;
	char * string_buffer;
	
	Elf64_Rela rela_buffer;
	Elf64_Sym sym_buffer;

	libc_func_t temp_func;

	
	string_buffer = malloc(FUNC_NAME_MAX);

	//For every dynamically linked function
	for (int i = 0; i < relaplt_header->sh_size / sizeof(Elf64_Rela); ++i) {

		//Get next .rela.plt symbol and find which index in .dynsym it uses
		fseek(target_file, relaplt_header->sh_offset + i*sizeof(Elf64_Rela), SEEK_SET);
		ret = fread(&rela_buffer, sizeof(Elf64_Rela), 1, target_file);
		temp_info = rela_buffer.r_info >> 32;

		//Get the .dynsym symbol for the above .rela.plt entry.
		fseek(target_file, dynsym_header->sh_offset + temp_info*sizeof(Elf64_Sym), SEEK_SET);
		ret = fread(&sym_buffer, sizeof(Elf64_Sym), 1, target_file);
		
		//The .dynsym symbol refers to a string in .dynstr, get it
		fseek(target_file, dynstr_header->sh_offset + sym_buffer.st_name, SEEK_SET);
		ret = fread(string_buffer, FUNC_NAME_MAX, 1, target_file);

		//We now have string, offset can be calculated with (i+1) * 0x10 + .plt offset
		strncpy(temp_func.name, string_buffer, FUNC_NAME_MAX);
		temp_func.offset = plt_header->sh_offset + 0x10*(i+1);

		vector_ret = vector_add(func_vector, 0, (char *) &temp_func, VECTOR_APPEND_TRUE);
		if (vector_ret) {
			printf("vector error, return code: %d\n", vector_ret);
		}
	
	}

	free(string_buffer);
	return 0;
}


//read dynsym section header
void get_func_vector(Elf64_Ehdr * elf_header, FILE * target_file, vector_t * func_vector) {

	size_t ret;
	char * string_buffer;
	int section_count = 0;

	Elf64_Shdr temp_header;

	Elf64_Shdr shstrtab_header;
	Elf64_Shdr dynsym_header;
	Elf64_Shdr dynstr_header;
	Elf64_Shdr strtab_header;
	Elf64_Shdr plt_header;
	Elf64_Shdr relaplt_header;

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

			//match found for .plt
			if (!strncmp(string_buffer, ".plt", 5)) {
				memcpy(&plt_header, &temp_header, sizeof(Elf64_Shdr));
			}

		//potential .rela.plt
		} else if (temp_header.sh_type == SHT_RELA) {

			fseek(target_file, shstrtab_header.sh_offset + temp_header.sh_name, SEEK_SET);
			ret = fread(string_buffer, FUNC_NAME_MAX, 1, target_file);

			//match found for .rela.plt
			if(!strncmp(string_buffer, ".rela.plt", 10)) {
				memcpy(&relaplt_header, &temp_header, sizeof(Elf64_Shdr));
			}

		//if dynsym found
		} else if (temp_header.sh_type == SHT_DYNSYM) {
			memcpy(&dynsym_header, &temp_header, sizeof(Elf64_Shdr));

			//get dynstr
			fseek(target_file, elf_header->e_shoff + dynsym_header.sh_link*sizeof(Elf64_Shdr), SEEK_SET);
			ret = fread(&dynstr_header, sizeof(Elf64_Shdr), 1, target_file);
		}

	} //end for


	ret = populate_func_vector(&plt_header, &relaplt_header, &dynsym_header, &dynstr_header, func_vector, target_file);


	//TODO test
	
	libc_func_t test;
	for (int i = 0; i < func_vector->length; ++i) {
		vector_get(func_vector, i, (char *) &test);
		printf("0x%lx - %s\n", test.offset, test.name);
	}

	//TODO end test

	free(string_buffer);
}
