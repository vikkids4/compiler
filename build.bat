@echo off
call clean
bison -yd main.y
flex main.l
gcc -c lex.yy.c y.tab.c
gcc lex.yy.o y.tab.o -o main.exe
echo. 2>tokens.txt
echo. 2>log.txt
echo. 2>assembly.txt
echo.
echo Done