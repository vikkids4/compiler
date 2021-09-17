#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>

int classId = 0;
int functionId = 0;
int variableId = 0;
FILE *symTableFilePtr;

typedef struct{
	char name[10];
    char lineno[10];
    char *memoryLocation;
} classSym;

typedef struct{
	char name[10];
    char type[10];
    char lineno[10];
    char *memoryLocation;
} functionSym;

typedef struct{
	char name[10];
    char type[10];
    char size[10];
    char value[10];
    char lineno[10];
    char *memoryLocation;
} variableSym;

classSym classSymbolTable[50];
functionSym functionSymbolTable[50];
variableSym variableSymbolTable[50];

FILE *getSymTableFile() {
    symTableFilePtr = fopen("symbolTables.txt", "a");
    if (symTableFilePtr == NULL) {
        fprintf(symTableFilePtr, "[ERROR] Symbol Table File Not Found!");
        exit(1);
    }
}

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
    printf("-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n");
    printf("Variable Symbol Table");
    fprintf(symTableFilePtr,"Variable Symbol Table");
    printf("\n-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"\n-----------------------------------------------------------------------------------------\n");
    printf("%-12s%-12s%-12s%-12s%-12s%-12s%-15s\n", "No.", "Name", "Type", "Size", "Value", "Line No.", "Memory Location");
    fprintf(symTableFilePtr,"%-12s%-12s%-12s%-12s%-12s%-12s%-15s\n", "No.", "Name", "Type", "Size", "Value", "Line No.", "Memory Location");

	for(i=0; i < variableId; i++) {
        printf("%-12d%-12s%-12s%-12s%-12s%-12s%-15u\n", i, 
                variableSymbolTable[i].name, 
                variableSymbolTable[i].type, 
                variableSymbolTable[i].size,
                variableSymbolTable[i].value, 
                removeSpaces(variableSymbolTable[i].lineno),
                variableSymbolTable[i].memoryLocation);
        fprintf(symTableFilePtr,"%-12d%-12s%-12s%-12s%-12s%-12s%-15u\n", i, 
                variableSymbolTable[i].name, 
                variableSymbolTable[i].type, 
                variableSymbolTable[i].size,
                variableSymbolTable[i].value,
                removeSpaces(variableSymbolTable[i].lineno), 
                variableSymbolTable[i].memoryLocation);
	}

    printf("-----------------------------------------------------------------------------------------\n\n\n\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n\n\n\n");
}

void printClassSymbolTable() {
    int i=0;
    printf("-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n");
    printf("Class Symbol Table");
    fprintf(symTableFilePtr,"Class Symbol Table");
    printf("\n-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"\n-----------------------------------------------------------------------------------------\n");
    printf("%-12s%-12s%-12s\n", "No.", "Name", "Line No.");
    fprintf(symTableFilePtr,"%-12s%-12s%-12s\n", "No.", "Name", "Line No.");

	for(i=0; i < classId; i++) {
        printf("%-12d%-12s%-12s\n", i, classSymbolTable[i].name, removeSpaces(classSymbolTable[i].lineno));
        fprintf(symTableFilePtr,"%-12d%-12s%-12s\n", i, classSymbolTable[i].name, removeSpaces(classSymbolTable[i].lineno));
	}

    printf("-----------------------------------------------------------------------------------------\n\n\n\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n\n\n\n");
}

void printFunctionSymbolTable() {
    int i=0;
    printf("-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n");
    printf("Function Symbol Table");
    fprintf(symTableFilePtr,"Function Symbol Table");
    printf("\n-----------------------------------------------------------------------------------------\n");
    fprintf(symTableFilePtr,"\n-----------------------------------------------------------------------------------------\n");
    printf("%-12s%-12s%-12s%-12s\n", "No.", "Name", "Type", "Line No.");
    fprintf(symTableFilePtr,"%-12s%-12s%-12s%-12s\n", "No.", "Name", "Type", "Line No.");

	for(i=0; i < functionId; i++) {
        printf("%-12d%-12s%-12s%-12s\n", i, functionSymbolTable[i].name, functionSymbolTable[i].type, removeSpaces(functionSymbolTable[i].lineno));
        fprintf(symTableFilePtr,"%-12d%-12s%-12s%-12s\n", i, functionSymbolTable[i].name, functionSymbolTable[i].type, removeSpaces(functionSymbolTable[i].lineno));
	}

    printf("-----------------------------------------------------------------------------------------\n\n\n\n");
    fprintf(symTableFilePtr,"-----------------------------------------------------------------------------------------\n\n\n\n");
}

char *getVarLocation(char *v) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v) == 0){
            return variableSymbolTable[i].memoryLocation;
        }
    }
}

char *getVarType(char *v) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v) == 0){
            return variableSymbolTable[i].type;
        }
    }
}

char *getVarSize(char *v) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v) == 0){
            return variableSymbolTable[i].size;
        }
    }
}

variableSym *getVarriableSymbol(char *varName) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, varName) == 0){
            variableSym *v;
            strcpy(v->name,variableSymbolTable[i].name);
            strcpy(v->type,variableSymbolTable[i].type);
            strcpy(v->size,variableSymbolTable[i].size);
            strcpy(v->value,variableSymbolTable[i].value);
            return v;
        }
    }
}

int checkVariable(char *v, char *lineno) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v) == 0) {
            addParserLog("Duplicate variable definition detected...");
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

void updateVariableSymbolTableValue(variableSym *v) {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        if (strcmp(variableSymbolTable[i].name, v->name) == 0) {
            strcpy(variableSymbolTable[i].name, v->name);
            strcpy(variableSymbolTable[i].type, v->type);
            strcpy(variableSymbolTable[i].size, v->size);
            strcpy(variableSymbolTable[i].value, v->value);
        }
    }
}

void deallocateVariableMemory() {
    int i=0;
    for(i=0; i < variableId + 1; i++) {
        free(variableSymbolTable[i].memoryLocation);
        variableSymbolTable[i].memoryLocation = NULL;
    }
}

int insertVariableSymbolTableValue(variableSym v, char *lineno) {
    if (checkVariable(removeSpaces(v.name), lineno) == 0) {
        return 0;
    }
    char *var_value;
    if (v.size != NULL) {
        var_value = (char *) malloc(atoi(v.size) * 10);
    } else {
        var_value = (char *) malloc(10);
    }
    
    if (var_value == NULL) {
        printf("Memory overflow");
    }
    strcpy(var_value, removeSpaces(v.value));
    strcpy(variableSymbolTable[variableId].name, removeSpaces(v.name));
    strcpy(variableSymbolTable[variableId].type, removeSpaces(v.type));
    strcpy(variableSymbolTable[variableId].size, removeSpaces(v.size));
    strcpy(variableSymbolTable[variableId].value, removeSpaces(v.value));
    strcpy(variableSymbolTable[variableId].lineno, lineno);
    variableSymbolTable[variableId].memoryLocation = var_value;
    variableId++;
}

int insertClassSymbolTableValue(classSym c, char *lineno) {
    if (checkClass(removeSpaces(c.name), lineno) == 0)
    {
        return 0;
    }
    strcpy(classSymbolTable[classId].name, removeSpaces(c.name));
    strcpy(classSymbolTable[classId].lineno, lineno);
    classId++;
}

int insertFunctionSymbolTableValue(functionSym f, char *lineno) {
    if (checkFunction(removeSpaces(f.name), lineno) == 0)
    {
        return 0;
    }
    strcpy(functionSymbolTable[functionId].name, removeSpaces(f.name));
    strcpy(functionSymbolTable[functionId].type, removeSpaces(f.type));
    strcpy(functionSymbolTable[functionId].lineno, lineno);
    functionId++;
}

void printAllSymbolTables() {
    printInfo("Symbol Tables");
    getSymTableFile();
    printVariableSymbolTable();
    printClassSymbolTable();
    printFunctionSymbolTable();
}