#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {

	char * uwu = "Lain said:";
	char * uwu2 = "to the ELFes.";
	
	char * x = malloc(50);
	strcpy(x, uwu);
	free(x);

	puts("\"nothing to see here...\"");

	char * y = malloc(50);
	strcpy(y, uwu2);
	free(y);
	return 0;
}
