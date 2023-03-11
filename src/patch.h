#ifndef PATCH_H
#define PATCH_H

#include "setup.h"
#include "vector.h"

void patch_elf(vector_t func_vector, repl_t sub_table, Elf64_Ehdr elf_header, Elf64_Shdr text_header, FILE * target_file);

#endif
