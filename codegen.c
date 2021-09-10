#include <stdio.h>
#include <string.h>

FILE *assemblyFilePtr;

void generateCode(char *code) {
    assemblyFilePtr = fopen("log.txt", "a");
    if (assemblyFilePtr == NULL) {
        printf("[ERROR] Assembly File Not Found!");
        exit(1);
    }
	fprintf(assemblyFilePtr, "%s", code);
	fclose(assemblyFilePtr);
}