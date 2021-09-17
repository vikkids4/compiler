#include <stdio.h>
#include <string.h>

int tokenCount = 0;

void printToken(char *type, char *token, int lineno) {
	printf("Type: %-8s \t Token: %-12s Line no: %-12d \n", type, token, lineno);
}

void writeTokenOutput(char *type, char *token, int lineno) {
	FILE *fptr;
	fptr = fopen("tokens.txt", "a");
	if (fptr == NULL) {
		printf("[ERROR] Token Output File Not Found!");
		exit(1);
	}
	fprintf(fptr, "%s:%s:%d\n", type, token, lineno);
	printToken(type, token, lineno);
	fclose(fptr);
}