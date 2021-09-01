#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>

int classId = 0;
int functionId = 0;
int variableId = 0;

typedef struct{
	char name[10];
    int *memoryLocation;
} classSym;

typedef struct{
	char name[10];
    char type[10];
    int *memoryLocation;
} functionSym;

typedef struct{
	char name[10];
    char type[10];
    char size[10];
    char value[10];
    int *memoryLocation;
} variableSym;

classSym classSymbolTable[50];
functionSym functionSymbolTable[50];
variableSym variableSymbolTable[50];

char *removeSpaces(char *str) {
    char *inp = str, *outp = str;
    int prevSpace = 0;

    while (*inp) {
        if (isspace(*inp)) {
            if (!prevSpace) {
                *outp++ = ' ';
                prevSpace = 1;
            }
        } else {
            *outp++ = *inp;
            prevSpace = 0;
        }
        ++inp;
    }
    *outp = '\0';
    return str;
}

void printVariableSymbolTable() {
    int i=0;
    printf("\n\n==============================================================\n\n");
    printf("Variable Symbol Table");
    printf("\n\n==============================================================\n\n");
    printf("%-12s%-12s%-12s%-12s%-12s\n", "No.", "Name", "Type", "Size", "Value", "Memory Loc");

	for(i=0; i < variableId; i++) {
        printf("%-12d%-12s%-12s%-12s%-12s\n", i, variableSymbolTable[i].name, variableSymbolTable[i].type, variableSymbolTable[i].size, "");
	}

    printf("\n\n==============================================================\n\n");
}

void printClassSymbolTable() {
    int i=0;
    printf("\n\n==============================================================\n\n");
    printf("Class Symbol Table");
    printf("\n\n==============================================================\n\n");
    printf("%-12s%-12s%-12s\n", "No.", "Name", "Memory Loc");

	for(i=0; i < classId; i++) {
        printf("%-12d%-12s%-12s\n", i, classSymbolTable[i].name, "");
	}

    printf("\n\n==============================================================\n\n");
}

void printFunctionSymbolTable() {
    int i=0;
    printf("\n\n==============================================================\n\n");
    printf("Function Symbol Table");
    printf("\n\n==============================================================\n\n");
    printf("%-12s%-12s%-12s%-12s\n", "No.", "Name", "Type", "Memory Loc");

	for(i=0; i < functionId; i++) {
        printf("%-12d%-12s%-12s%-12s\n", i, functionSymbolTable[i].name, functionSymbolTable[i].type, "");
	}

    printf("\n\n==============================================================\n\n");
}

int checkVariable(char *v, char *lineno) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v) == 0)
        {
            printParserLog("Duplicate variable definition detected...");
            addErrorLog("Duplicate variable definition", lineno);
            return 0;
        }
        
    }
    return 1;
}

int checkClass(char *c, char *lineno) {
    int i=0;
    for(i=0; i < classId + 1; i++) {
        if (strcmp(classSymbolTable[i].name, c) == 0)
        {
            addErrorLog("Duplicate class definition", lineno);
            return 0;
        }
    }
    return 1;
}

int checkFunction(char *f, char *lineno) {
    int i=0;
    for(i=0; i < functionId + 1; i++) {
        if (strcmp(functionSymbolTable[i].name, f) == 0)
        {
            addErrorLog("Duplicate function definition", lineno);
            return 0;
        }
    }
    return 1;
}

int updateVariableSymbolTable(variableSym v, char *lineno) {
    if (checkVariable(removeSpaces(v.name), lineno) == 0)
    {
        return 0;
    }
    
    strcpy(variableSymbolTable[variableId].name, removeSpaces(v.name));
    strcpy(variableSymbolTable[variableId].type, removeSpaces(v.type));
    strcpy(variableSymbolTable[variableId].size, removeSpaces(v.size));
    strcpy(variableSymbolTable[variableId].value, removeSpaces(v.value));
    variableId++;
}

int updateClassSymbolTable(classSym c, char *lineno) {
    if (checkClass(removeSpaces(c.name), lineno) == 0)
    {
        return 0;
    }
    strcpy(classSymbolTable[classId].name, removeSpaces(c.name));
    classId++;
}

int updateFunctionSymbolTable(functionSym f, char *lineno) {
    if (checkFunction(removeSpaces(f.name), lineno) == 0)
    {
        return 0;
    }
    strcpy(functionSymbolTable[functionId].name, removeSpaces(f.name));
    strcpy(functionSymbolTable[functionId].type, removeSpaces(f.type));
    functionId++;
}

void printAllSymbolTables() {
    printVariableSymbolTable();
    printClassSymbolTable();
    printFunctionSymbolTable();
}