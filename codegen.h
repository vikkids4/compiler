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


void addScopeElements(char *element, int scope) {
    strcpy(scopes[scope][scopesArrayLegnths[scope]].element, element);
    scopesArrayLegnths[scope] = scopesArrayLegnths[scope] + 1;
}

FILE *getFile() {
    assemblyFilePtr = fopen("assembly.txt", "a");
    if (assemblyFilePtr == NULL) {
        printf("[ERROR] Assembly File Not Found!");
        exit(1);
    }
}

void printScopeElements() {
    getFile();
    int i=0;
    for(i=0; i < 10; i++) {
        int j=0;
        for(j=0; j < scopesArrayLegnths[i]; j++) {
            fprintf(assemblyFilePtr, "%s", scopes[i][j].element);
        }
    }
}

void generateString(char str[100], int scope) {
    assemblyFilePtr = getFile();
    addScopeElements(str, scope);
}

void generateLabelCode(char label[10], int scope) {
    assemblyFilePtr = getFile();
    addScopeElements(label, scope);
    addScopeElements(":", scope);
    addScopeElements("\n", scope);
}

void generateNoOperandCode(char opcode[20], int scope) {
    assemblyFilePtr = getFile();
    addScopeElements("\t", scope);
    addScopeElements(opcode, scope);
    addScopeElements("\n", scope);
}

void generateOneOperandCode(OneOperand op, int scope) {
    assemblyFilePtr = getFile();
    addScopeElements("\t", scope);
    addScopeElements(op.opcode, scope);
    addScopeElements(" ", scope);
    addScopeElements(op.operand, scope);
    addScopeElements("\n", scope);
}

void generateTwoOperandCode(TwoOperand op, int scope) {
    assemblyFilePtr = getFile();
    addScopeElements("\t", scope);
    addScopeElements(op.opcode, scope);
    addScopeElements(" ", scope);
    addScopeElements(op.operand1, scope);
    addScopeElements(" ", scope);
    addScopeElements(op.operand2, scope);
    addScopeElements("\n", scope);
}