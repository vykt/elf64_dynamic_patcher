# elf64_dynamic_patcher


### ABOUT:

This is a static (on disk) executable patcher for dynamically linked 64bit ELFs. It 
works by resolving function headers using sections necessary for execution. As a result 
this patcher works on stripped executables.


### BUILD:

Fetch the repo:
```
$ clone https://github.com/vykt/elf64_dynamic_patcher
```

Build the patcher & target:
```
$ cd elf64_dynamic_patcher/src
$ make && make target
```

### RUN:

This repo comes with an example that changes the call to free() to a call to puts().
Feel free to experiment with the code and create your own executables to patch.

First, run the target before the patch:
```
$ ./target
```

Next, patch the target:
```
$ ./elf_patcher target free puts
```

Run the target again after the patch:
```
$ ./target
```


### HOW DOES THIS WORK?

For details on the ELF format and its various headers see <i>man 5 elf</i>. This patcher 
conducts the work of a linker in resolving symbols found in <i>.dynsym</i> and 
<i>.dynstr</i> sections to the the dynamic function stubs found in the <i>.plt</i> 
section.

Once the addresses of the dynamic function stubs are known, it's possible to change 
the relative offset of the call instruction in the <i>.text</i> section of the program. 
For example, if the <i>free()</i> stub is at address 0x20 and the <i>puts()</i> stub is 
at address 0x30, it's possible to add 0x10 to the call offset in .text to change the call 
from <i>free()</i> to <i>puts()</i>


### WHEN IS THIS USEFUL?

This patcher is useful for printing the contents of dynamically allocated buffers as is 
done in the provided example. Patching functions that take different parameters is 
far beyond the scope of this project and is best done by hand using something like 
radare2.
