@echo off
echo.
echo Cleaning Files Before Starting Compiling
echo ----------------------------------------
echo Deleting all exe files
del *.exe
echo Deleting lex.yy.c
del lex.yy.c 
echo Deleting y.tab.c
del y.tab.c
echo Deleting all out files
del *.o
echo Deleting y.tab.h
del y.tab.h
echo Deleting tokens.txt
del tokens.txt
echo Deleting log.txt
del log.txt
echo Deleting assembly.txt
del assembly.txt
echo Deleting symbolTables.txt
del symbolTables.txt