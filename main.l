%{
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "y.tab.h"
void yyerror(char *);
extern int yylineno;
%}

%option yylineno

LETTER      [a-z]|[A-Z]
DIGIT       [0-9]
NONZERO     [1-9]+
ALPHANUM    {LETTER}|{DIGIT}
ID          {LETTER}{ALPHANUM}*
FRACTION    {DIGIT}*{NONZERO}|.0
NUM         {NONZERO}{DIGIT}*{FRACTION}|0{FRACTION}
INTEGER     {NONZERO}{DIGIT}*

%%

"*"         {writeTokenOutput("OPERATOR", yytext, yylineno); return ASTK;}
"/"         {writeTokenOutput("OPERATOR", yytext, yylineno); return FS;}
"%"         {writeTokenOutput("OPERATOR", yytext, yylineno); return PCT;}
"+"         {writeTokenOutput("OPERATOR", yytext, yylineno); return PLUS;}
"-"         {writeTokenOutput("OPERATOR", yytext, yylineno); return MINUS;}
"."         {writeTokenOutput("OPERATOR", yytext, yylineno); return DOT;} 
"="         {writeTokenOutput("OPERATOR", yytext, yylineno); return EQ;}
")"         {writeTokenOutput("OPERATOR", yytext, yylineno); return CB;}
"("         {writeTokenOutput("OPERATOR", yytext, yylineno); return OB;}
("{"|"<%")  {writeTokenOutput("OPERATOR", yytext, yylineno); return OP;}
("}"|">%")  {writeTokenOutput("OPERATOR", yytext, yylineno); return CP;}
("["|"<:")	{writeTokenOutput("OPERATOR", yytext, yylineno); return OSB;}
("]"|":>")  {writeTokenOutput("OPERATOR", yytext, yylineno); return CSB;}
"<"         {writeTokenOutput("OPERATOR", yytext, yylineno); return LT;}
">"         {writeTokenOutput("OPERATOR", yytext, yylineno); return GT;}
"<="        {writeTokenOutput("OPERATOR", yytext, yylineno); return LE;}
"<>"        {writeTokenOutput("OPERATOR", yytext, yylineno); return LG;}
"=="        {writeTokenOutput("OPERATOR", yytext, yylineno); return EQQ;}
"!="        {writeTokenOutput("OPERATOR", yytext, yylineno); return NE;}
","         {writeTokenOutput("OPERATOR", yytext, yylineno); return CMA;}
";"         {writeTokenOutput("OPERATOR", yytext, yylineno); return SC;}
"/*"        {writeTokenOutput("OPERATOR", yytext, yylineno); return CMTSTART;}
"*/"        {writeTokenOutput("OPERATOR", yytext, yylineno); return CMTEND;}
"//"        {writeTokenOutput("OPERATOR", yytext, yylineno); return CMT;}

"while"     {writeTokenOutput("RESERVED", yytext, yylineno); return WHILE;}
"if"        {writeTokenOutput("RESERVED", yytext, yylineno); return IF;}
"else"      {writeTokenOutput("RESERVED", yytext, yylineno); return ELSE;}
"do"        {writeTokenOutput("RESERVED", yytext, yylineno); return DO;}
"print"     {writeTokenOutput("RESERVED", yytext, yylineno); return PRINT;}
"class"     {writeTokenOutput("RESERVED", yytext, yylineno); return CLASS;}
"integer"   {writeTokenOutput("RESERVED", yytext, yylineno); return INT;}
"real"      {writeTokenOutput("RESERVED", yytext, yylineno); return REAL;}
"read"      {writeTokenOutput("RESERVED", yytext, yylineno); return READ;}
"write"     {writeTokenOutput("RESERVED", yytext, yylineno); return WRITE;}
"return"    {writeTokenOutput("RESERVED", yytext, yylineno); return RETURN;}
"and"       {writeTokenOutput("RESERVED", yytext, yylineno); return AND;}
"not"       {writeTokenOutput("RESERVED", yytext, yylineno); return NOT;}
"or"        {writeTokenOutput("RESERVED", yytext, yylineno); return OR;}

<<EOF>>      {writeTokenOutput("RESERVED", "EOF", yylineno); return 0;}

{LETTER}    {writeTokenOutput("LETTER", yytext, yylineno); return LETTER;}
{DIGIT}     {writeTokenOutput("DIGIT", yytext, yylineno); return DIGIT;}
{NONZERO}   {writeTokenOutput("NONZERO", yytext, yylineno); return NONZERO; }
{ALPHANUM}  {writeTokenOutput("ALPHANUM", yytext, yylineno); return ALPHANUM;}
{ID}        {writeTokenOutput("ID", yytext, yylineno); return ID;}
{FRACTION}  {writeTokenOutput("FRACTION", yytext, yylineno); return FRACTION;}
{NUM}       {writeTokenOutput("NUM", yytext, yylineno); return NUM;}
{INTEGER}   {writeTokenOutput("INTEGER", yytext, yylineno); return INTEGER;}

[ \n]+    ;       /* ignore whitespace */ 
[ \t\n]+    ;       /* ignore whitespace */ 

.               yyerror("Unknown character");

%%
int yywrap(void) {
    return 1;
}

// int main() {
//     printf("Enter a string: ");
//     yylex();
// }