%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#include "parser.h"

/* prototypes */
// nodeType *cls(char* value);
// nodeType *var(char* type, char* name);

/* symbol tables */
// struct symbols clsSymbTable[20];
// struct symbols varSymbTable[20];

/* symbol table ids */
int clsId;
int varId;

int yylex(void);
void yyerror(char *s);
%}

// %union {
//     char* id;
//     int iValue;                 /* integer value */
//     char *mLocation;            /* symbol table index */
//     nodeType *nPtr;             /* node pointer */
//     char *symName;              /* symbol value */
// };

%token OP CP OSB CSB DOT OB CB SC
%token ASTK FS PCT
%token PLUS MINUS
%token LT GT LE GE
%token EQ EQQ
%token CMA

%token LETTER DIGIT NONZERO ALPHANUM ID FRACTION NUM INTEGER
%token WHILE IF THEN DO CLASS PROGRAM
%nonassoc ELSE

%token LG NE CMTSTART CMTEND CMT 
%token PRINT READ REAL INT WRITE RETURN AND NOT OR

%%

prog:;

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
	exit(0);
}

int main(void) {
    FILE *fptr;
    extern FILE *yyin;
	fptr = fopen("input.txt", "r");
	if (fptr == NULL) {
		printf("[ERROR] Token Output File Not Found!");
		exit(1);
	}

    printPhase("Lexical Analysis - Token Identification");
    
    yyin = fptr;
    while(yylex() != 0) {}
    
    fclose(fptr);
    parseStart();
    return 0;
}