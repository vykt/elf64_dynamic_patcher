# elf64_dynamic_patcher


### ABOUT:

A static (on disk) executable patcher for dynamically linked 64-bit ELFs. Works by 
resolving function headers using the necessary-for-execution sections, then changing 
32-bit relative offsets for call instructions. Works on stripped executables.


### EXPLANATION:


```
$ man 5 elf
```

- .plt section holds stubs of dynamically linked functions. Code from .text will jump 
  here when calling these dynamic procedures.

- .rela.plt connects .plt with .dynsym section. This section is important as it is 
  necessary for execution of the program unlike .symtab, which can be freely stripped. 
  The 'info' entry of .rela.plt contains an index into the .dynsym segment.

- .dynsym contains symbols for dynamic linking. More importantly, the headers of this 
  section contain an index into the corresponding string table, .dynstr. This means 
  functions references in .plt can now be given a name.

- .dynstr is the string table for the symbols found in the dynsym section. Unlike that 
  section, .dynstr stores the string representations of the symbols.

Putting it all together, by following the chain of sections desctibed above, it is 
possible to find the string representation of every called function (e.g.: 'free()') 
from .dynstr''', and find its offset in the file from .plt. 

The 'call' instruction appears as '0xe8' in hex, followed by a 4 byte (uint32_t) 
relative address. it is added to the address of the last byte of the instruction 
including its operand. Subtraction is performed by overflowing the offset.

It is therefore possible to scan the .text segment of the ELF until encountering 
'0xe8', followed by a 4 byte offset that lands right at the start of one of the 
functions found in .plt. Since the string 'name' of every function in .plt is known, 
the function that is being 'called' is now known.

It is now possible to alter the offset to call a different function of one's choice. 
This patcher is programmed to find every instance of a call to 'free()' and to replace 
it with a call to 'puts()'. This works well as both functions take identical parameters.

Patching functions with different parameters automatically (rather than by hand with 
something like radare2) is beyond the scope of this program, or the time I have :p
