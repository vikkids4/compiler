#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *assemblyFilePtr;

typedef struct{
    const char element[100];
} elements;

elements scopes[100][100];
int scopesArrayLegnths[100];

typedef struct{
    char opcode[10];
	char operand[10];
} OneOperand;

typedef struct{
    char opcode[10];
	char operand1[10];
    char operand2[10];
} TwoOperand;

void resetScopes() {
    const size_t R = 100;
    const size_t C = 100;
    memset(scopes, 0, sizeof(scopes[0][0]) * R * C);
    memset(scopesArrayLegnths, 0, sizeof(scopesArrayLegnths[0]) * R);
}

FILE *getFile() {
    assemblyFilePtr = fopen("assembly.txt", "a");
    if (assemblyFilePtr == NULL) {
        printf("[ERROR] Assembly File Not Found!");
        exit(1);
    }
}

void printCode(char *code) {
    getFile();
    fprintf(assemblyFilePtr, "%s", code);
    printf("%s", code);
}

void generateString(char str[100]) {
    assemblyFilePtr = getFile();
    printCode(str);
}

void generateLabelCode(char label[10]) {
    assemblyFilePtr = getFile();
    printCode(label);
    printCode(":");
    printCode("\n");
}

void generateNoOperandCode(char opcode[20]) {
    assemblyFilePtr = getFile();
    printCode("\t");
    printCode(opcode);
    printCode("\n");
}

void generateOneOperandCode(OneOperand op) {
    assemblyFilePtr = getFile();
    printCode("\t");
    printCode(op.opcode);
    printCode(" ");
    printCode(op.operand);
    printCode("\n");
}

void generateTwoOperandCode(TwoOperand op) {
    assemblyFilePtr = getFile();
    printCode("\t");
    printCode(op.opcode);
    printCode(" ");
    printCode(op.operand1);
    printCode(" ");
    printCode(op.operand2);
    printCode("\n");
}