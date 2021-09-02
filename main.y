%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#include "main.h"
#include "parser.h"

/* prototypes */
nodeType *cls(char* value);
nodeType *var(char* type, char* name);

/* symbol tables */
struct symbols clsSymbTable[20];
struct symbols varSymbTable[20];

/* symbol table ids */
int clsId;
int varId;

int yylex(void);
void yyerror(char *s);
%}

%union {
    char* id;
    int iValue;                 /* integer value */
    char *mLocation;            /* symbol table index */
    nodeType *nPtr;             /* node pointer */
    char *symName;              /* symbol value */
};

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

%type <id> ID INTEGER REAL type
%type <nPtr> classDecl varDecl

%%

prog:
    classDecl progBody {printf("Correct syntax");}
    ;
classDecl:
    classDecl CLASS ID OP varDecl funcDef CP 
    | CLASS ID OP varDecl funcDef CP
    | CLASS ID OP CP {$$ = cls($2);}
    // | CLASS ID OP CP {updateClassSymbolTable($2); printf("value is %s", $2);} // this line is for testing, dummy grammar
    ;
progBody:
    PROGRAM funcBody SC funcDef
    ;
funcHead: 
    type ID OB fParams CB
    ;
funcDef:
    funcDef funcHead funcBody |
    funcHead funcBody
    ;
funcBody: 
    OP varDecl statement CP |
    ;
varDecl: 
    varDecl type ID arraySize SC |
    type ID arraySize SC |
    type ID SC {$$ = var($1, $2);} | //dummy grammar
    ;
statement: 
    statement variable assignOp expr SC |
    statement IF OB expr CB DO statBlock SC |
    statement READ OB variable CB SC |
    statement WRITE OB expr CB SC |
    statement RETURN OB expr CB |
    variable assignOp expr SC |
    IF OB expr CB DO statBlock SC |
    READ OB variable CB SC |
    WRITE OB expr CB SC |
    RETURN OB expr CB |
    ;
statBlock: 
    OP statement CP |
    variable assignOp expr SC |
    IF OB expr CB DO statBlock SC |
    READ OB variable CB SC |
    WRITE OB expr CB SC |
    RETURN OB expr CB SC
    ;
expr:
    arithExpr addOp term |
    term multOp factor |
    idnest ID indice |
    idnest ID OB aParams CB |
    NUM |
    OB expr CB |
    NOT factor |
    sign factor |
    arithExpr relOp arithExpr |
    ;
arithExpr: 
    arithExpr addOp term |
    term multOp factor |
    idnest ID indice |
    idnest ID OB aParams CB |
    NUM |
    OB expr CB |
    NOT factor |
    sign factor |
    ;
sign:
    PLUS | MINUS ;
term:
    term multOp factor |
    idnest ID indice |
    idnest ID OB aParams CB |
    OB expr CB |
    NOT factor |
    sign factor
    ;
factor: 
    idnest ID indice |
    idnest ID OB aParams CB |
    NUM |
    OB expr CB |
    NOT factor |
    sign factor
    ;
variable: 
    idnest ID indice
    ;
idnest:
    idnest ID indice |
    ID indice
    ;
indice: 
    indice OSB arithExpr CSB |
    OSB arithExpr CSB
    ;
arraySize: 
    arraySize OSB INTEGER CSB |
    OSB INTEGER CSB |
    OSB DIGIT CSB |
    OSB NONZERO CSB |
    OSB FRACTION CSB |
    OSB LETTER CSB
    ;
type: 
    INTEGER {$$ = $1;}|
    ID      {$$ = $1;}
    ;
fParams:
    type ID arraySize fParamsTail |
    ;
aParams: 
    expr aParamsTail |
    ;
fParamsTail:
    fParamsTail CMA type arraySize |
    CMA type arraySize |
    ;
aParamsTail:
    aParamsTail CMA expr |
    CMA expr
    ;

assignOp:   EQ;
relOp:      EQQ | LG | LT | GT | LE | GE ;
addOp:      PLUS | MINUS | OR ;
multOp:     ASTK | FS | AND ;

%%

nodeType *cls(char* name) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCls;
    p->cls.name = name;

    // printf("Class Name: %s\n", p->cls.name);

    return p;
}

nodeType *var(char* type, char* name) {
    
    // check whether symbol exists
    int c;
    for(c=0; c < varId; c++) {
        if(strcmp(varSymbTable[c].node.var.name , name) == 0) {
            yyerror("\n[ERROR] Duplicate class declaration!\n");
        }
    }

    nodeType *p;
    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeVar;
    p->var.name = name;
    p->var.type = type;

    // printf("Variable Name: %s\n", p->var.name);
    // printf("Variable Type: %s\n", p->var.type);
    varSymbTable[varId++].node = *p;

    printf("\n\nVariables Symbol Table\n\n");
    printf("%-12s%-12s%-12s\n", "No.", "Variable", "Value");
    for(c=0; c < varId; c++) {
        printf("%-12d%-12s%-12s\n", c, varSymbTable[c].node.var.name, "value");
    }

    return p;
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
	exit(0);
}

int main(void) {
    // yyparse();
    // int testInteger;
    // printf("Enter a token: ");
    // scanf("%d", &testInteger); 

    // while(1){
    //     char str[25];
    //     printf("Enter your token: ");
    //     int a = yylex();
    //     if(a == 0) {
    //         break;
    //     }
    // }

    FILE *fptr;
    extern FILE *yyin;
	fptr = fopen("input.txt", "r");
	if (fptr == NULL) {
		printf("[ERROR] Token Output File Not Found!");
		exit(1);
	}

    printPhase("Lexical Analysis - Token Identification");

    yyin = fptr;
    while(yylex() != 0) {

    }
    // yylex();
    fclose(fptr);

    parseStart();

    return 0;
}