@echo off
call clean

echo.
echo.
echo Starting To Compile The Compiler
echo --------------------------------

echo Compiling main.y file
bison -yd main.y
echo Compiling main.l file
flex main.l
echo Creating main.exe file
gcc -c lex.yy.c y.tab.c
gcc lex.yy.o y.tab.o -o main.exe

echo Creating Tokens Output File
echo Creating Logs Output File
echo Creating Assemboly Output File
echo Creating Symbol Tables Output File
echo. 2>tokens.txt
echo. 2>log.txt
echo. 2>assembly.txt
echo. 2>symbolTables.txt

echo Compiler compiled successfully
echo.