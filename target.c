#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {

	char * uwu = "hello";
	char * uwu2 = "goodbye";
	
	char * x = malloc(50);
	strcpy(x, uwu);
	free(x);

	puts("regular puts owo\n");

	char * y = malloc(50);
	strcpy(y, uwu2);
	free(y);
	return 0;
}
