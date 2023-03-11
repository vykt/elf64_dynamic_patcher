#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <elf.h>

#include <sys/mman.h>

#include "setup.h"
#include "vector.h"


//TODO debug includes
#include <errno.h>


//replace function calls in elf files according to sub_table
void patch_elf(vector_t func_vector, repl_t sub_table, Elf64_Ehdr elf_header, Elf64_Shdr text_header, FILE * target_file) {

	off_t page_size;
	off_t offset;
	off_t mmap_offset;
	int target_fd;

	uint8_t * text_section; //process .text a byte at a time
	
	uint8_t call = 232; //0xe8, call instruction
	uint32_t * call_addr;
	uint64_t iteration_offset;

	//get page size
	page_size = sysconf(_SC_PAGE_SIZE);

	//calculate appropriate offset
	offset = (off_t) page_size * (text_header.sh_offset / page_size);
	mmap_offset = text_header.sh_offset % page_size;

	//get file descriptor for target ELF using previously opened stream
	if ((target_fd = fileno(target_file)) == -1) {
		perror("mmap target ELF");
		exit(1);
	}

	//map .text section into memory, starting from nearest page until end of .text
	text_section = mmap(NULL, text_header.sh_size + mmap_offset, PROT_READ | PROT_WRITE, MAP_SHARED, target_fd, offset);
	if (text_section == MAP_FAILED) {
		perror("mmap allocation");
		exit(1);
	}

	iteration_offset = text_header.sh_offset;

	//for every byte of .text where full call instruction with 32bit offset would fit
	for (uint8_t * i = text_section + mmap_offset; i < text_section + mmap_offset + text_header.sh_size - 4; ++i) {
		
		//if found call
		if (*i == call) {

			//get pointer to the relative call operand
			call_addr = (signed int *) (i+1);

			//if the operand evaluates to a jump to target function in .plt
			if (iteration_offset + ((signed int) *call_addr) + 5 == sub_table.offset_old) {

				//make it call the new target function
				*call_addr = *call_addr + sub_table.diff;
			}
		}
		++iteration_offset;
		
	}

}
