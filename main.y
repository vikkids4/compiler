%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "parser.h"

/* symbol table ids */
int clsId;
int varId;

int yylex(void);
void yyerror(char *s);
%}

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

clock_t prog_start, lex_start, parse_start, codegen_start, prog_end, lex_end, parse_end, codegen_end;
double prog_time, lex_time, parse_time, codegen_time;

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
	exit(0);
}

void lexicalAnalysis() {
    bool enableTest = false;

    FILE *inputFilePtr;
    extern FILE *yyin;
    if (enableTest == true) {
        char inputFile[30];
        printf("\nPlease enter the test case file name: ");
        scanf("%s", &inputFile);
        strcat(inputFile, ".txt");
	    inputFilePtr = fopen(inputFile, "r");
    } else {
        inputFilePtr = fopen("input.txt", "r");
    }
	if (inputFilePtr == NULL) {
		printf("[ERROR] The Given Input File Not Found!");
		exit(1);
	}
    
    lex_start = clock();

    yyin = inputFilePtr;
    while(yylex() != 0) {}
    fclose(inputFilePtr);

    lex_end = clock();
    lex_time = ((double) (lex_end - lex_start)) / CLOCKS_PER_SEC;
}
void syntaxtParsing() {
    parse_start = clock();
    parseStart();
    parse_end = clock();
    parse_time = ((double) (parse_end - parse_start)) / CLOCKS_PER_SEC;
}
void codeGeneration() {
    codegen_start = clock();
    generateCode();
    codegen_end = clock();
    codegen_time = ((double) (codegen_end - codegen_start)) / CLOCKS_PER_SEC;
}

void printStatus() {
    printf("\n\n-------------------------------------------------------------------------------\n");
    printf("%-12s%-30s%-16s%-12s\n", "Phase No.", "Phase Name", "Status", "Time Elapsed (s)");
    printf("-------------------------------------------------------------------------------\n");
    printf("%-12s%-30s%-16s%-12f\n", "1", "Lexical Analysis", "Completed", lex_time);
    printf("%-12s%-30s%-16s%-12f\n", "2", "Syntax/Semantic Analysis", "Completed", parse_time);
    printf("%-12s%-30s%-16s%-12f\n", "3", "Code Generation", "Completed", codegen_time);
    printf("-------------------------------------------------------------------------------\n");
    printf("%35s%23s%-12f\n", "Overall Time", "", lex_time + parse_time + codegen_time);
    printf("-------------------------------------------------------------------------------\n\n");

    int errorsCount = getErrorsCount();
    printf("\nCompilation finished in %f seconds with %d errors.\n\n\n", prog_time, errorsCount);

    if (errorsCount > 0) {
        printErrorLogs();
    }
}

int main(void) {

    prog_start = clock();

    lexicalAnalysis();
    syntaxtParsing();
    codeGeneration();
    
    prog_end = clock();
    prog_time = ((double) (prog_end - prog_start)) / CLOCKS_PER_SEC;

    printStatus();
    return 0;
}