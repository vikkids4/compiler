#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "symbolTable.h"

FILE *tokensFilePtr;
FILE *logFilePtr;
char line[255];
char* token;
char* currToken;
char* currTokenType;
char* currLineNo;
int parserLogLine = 0;

void initFile() {
    tokensFilePtr = fopen("tokens.txt", "r");
    if (tokensFilePtr == NULL) {
        printf("[ERROR] Token Output File Not Found!");
        exit(1);
    }
}

char *getNextToken() {
    token = fgets(line, 255, tokensFilePtr);
    currTokenType = strtok(token, ":");  
    currToken = strtok(NULL, ":");
    currLineNo = strtok(NULL, ":"); 
}

void printParserLog(char *log) {
    if(parserLogLine > 200) {
        exit(0);
    }
    printf("[%d] %s\n", ++parserLogLine, log);

    logFilePtr = fopen("log.txt", "a");
    if (logFilePtr == NULL) {
        printf("[ERROR] Token Output File Not Found!");
        exit(1);
    }
	fprintf(logFilePtr, "[%d] %s\n", parserLogLine, log);
	fclose(logFilePtr);
}

int type() {
    if (strcmp(currToken, "integer") == 0 || 
        strcmp(currToken, "real") == 0    || 
        strcmp(currToken, "id") == 0) {
        printParserLog("Valid type detected...");
        return 1;
    } else {
        // printf("TOKEN: %s\n", currToken);
        printParserLog("Invalid type detected...");
        addErrorLog("Only the types integer, real are allowed", currLineNo);
        return 0;
    }
}

int varDecl(variableSym variable) {

    getNextToken();

    if (strcmp(currTokenType, "DIGIT") == 0   || 
        strcmp(currTokenType, "INTEGER") == 0 || 
        strcmp(currTokenType, "NUM") == 0) {
        strcpy(variable.size, currToken);

        getNextToken(); 

        if (strcmp(currToken, "]") == 0) {
            // insert into variable symbol table
            printParserLog("Variable declaration successful!");
            if(updateVariableSymbolTable(variable, currLineNo) == 0) {
                printParserLog("Update variable symbol table failed...");
                return 0;
            }
            return 1;
        } else {
            addErrorLog("Invalid variable declaration: expected a ]", currLineNo);
            return 0;
        }
    }         
}

int funcHead() {
    return 1;
}

int addOp() {
    if (strcmp(currToken, "+") == 0 ||
        strcmp(currToken, "-") == 0 ||
        strcmp(currToken, "or") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int multOp() {
    if (strcmp(currToken, "*") == 0 ||
        strcmp(currToken, "/") == 0 ||
        strcmp(currToken, "and") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int relOp() {
    printf("REL OP CHECK: %s\n", currToken);
    if (strcmp(currToken, "==") == 0 ||
        strcmp(currToken, "<>") == 0 ||
        strcmp(currToken, "<") == 0 ||
        strcmp(currToken, ">") == 0 ||
        strcmp(currToken, "<=") == 0 ||
        strcmp(currToken, ">=") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int assignOp() {
    if (strcmp(currToken, "=") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int indice() {
    printParserLog("Checking for an indice");
    if(strcmp(currToken, "[") == 0) {
        getNextToken();
        printParserLog("Checking for an arithmatic expression");
        if (arithExpr() == 1) {
            printParserLog("Arithmatic expression detected");
            getNextToken();
            if (strcmp(currToken, "]") == 0) {
                printParserLog("Indice declared succesfully");
                return 1;
            } else {
                printParserLog("Invalid indice definition, expected a ] after arithmatic expression");
                addErrorLog("Invalid indice definition, expected a ] after arithmatic expression", currLineNo);
                return 0;
            }   
        } else {
            printParserLog("Invalid indice definition, expected an arithmatic expression after [");
            addErrorLog("Invalid indice definition, expected an arithmatic expression after [", currLineNo);
            return 0;
        }
    } else {
        printParserLog("Invalid indice definition, should start with [");
        return 0;
    }
}

int idnest() {
    printParserLog("Checking for an idnest");
    if(strcmp(currTokenType, "ID") == 0) {
        printParserLog("Valid ID detected inside idnest, checking for indice");
        getNextToken();
        if(indice() == 1) {
            printParserLog("Valid indice detected, checking for more indice definition if available");
            getNextToken();
            while (indice() == 1) {
                printParserLog("Looping indices inside idnest");
                getNextToken();
            }
            if(strcmp(currToken, ".") == 0) {
                printParserLog("Valid idnest definition found");
                return 1;
            }
        }
    } else {
        printParserLog("Invalid idnest definition, should start with an ID");
        return 0;
    }
}

int variable() {
    printParserLog("Checking for a variable");
    if(idnest() == 1) {
        printParserLog("Valid idnest detected, checking for more idnest definition if available");
        while(idnest() == 1) {
            printParserLog("More idnest definition found");
            getNextToken();
        }
    } 
    if(strcmp(currTokenType, "ID") == 0) {
        printParserLog("Valid ID detected inside variable, checking for indice");
        if(indice() == 1) {
            printParserLog("Valid indice detected, checking for more indice definition if available");
            getNextToken();
            while (indice() == 1) {
                printParserLog("Looping indices inside idnest");
                getNextToken();
            }
            // no need to getNextToken() after this method
            return 1;
        }
    } else {
        printParserLog("No variable definitions found");
        return 0;
    }
}

int sign() {
    printParserLog("Checking for sign");
    if (strcmp(currToken, "+") == 0 ||
        strcmp(currToken, "-") == 0) {
        return 1;
    } else {
        printParserLog("Sign check failed");
        return 0;
    }
}

int factor() {
    if (variable() == 1) {
        // no need to getNextToken() afer this
        printParserLog("Valid variable definition found, factor defintion success");
        return 1;
    } else if (idnest() == 1) {
        printParserLog("Valid idnest detected, checking for more idnest definition if available");
        while(idnest() == 1) {
            printParserLog("More idnest definition found");
            getNextToken();
        }
        if(strcmp(currTokenType, "ID") == 0) {
            printParserLog("A valid ID detected, checking (aParamas)");
            getNextToken();
            if(strcmp(currToken, "(") == 0) {
                printParserLog("Checking for aParams");
                getNextToken();
                while (strcmp(currToken, ")") == 0){
                    if(aParams() == 1) {
                        printParserLog("aParams detected, checking for more");
                        getNextToken();
                        while(aParams() == 1) {
                            printParserLog("More aParams detected");
                            getNextToken();
                        }
                    }

                    if(strcmp(currToken, ")") == 0) {
                        printParserLog("Valid factor definition found");
                        return 1;
                    } else {
                        getNextToken();
                    }
                }
                
            } else {
                printParserLog("Invalid Factor definition, an ID should be followed by (aParams)");
                addErrorLog("Invalid Factor definition, an ID should be followed by (aParams)", currLineNo);
                return 0;
            }

        } else {
            printParserLog("Invalid Factor definition, an idnest should be followed by an ID");
            addErrorLog("Invalid Factor definition, an idnest should be followed by an ID", currLineNo);
            return 0;
        }
    } else if(strcmp(currTokenType, "ID") == 0) {
        printParserLog("A valid ID detected, checking (aParamas)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Checking for aParams");
            getNextToken();
            while (strcmp(currToken, ")") == 0){
                if(aParams() == 1) {
                    printParserLog("aParams detected, checking for more");
                    getNextToken();
                    while(aParams() == 1) {
                        printParserLog("More aParams detected");
                        getNextToken();
                    }
                }

                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Valid factor definition found");
                    return 1;
                } else {
                    getNextToken();
                }
            }
            
        } else {
            printParserLog("Invalid Factor definition, an ID should be followed by (aParams)");
            addErrorLog("Invalid Factor definition, an ID should be followed by (aParams)", currLineNo);
            return 0;
        }

    } else if(strcmp(currTokenType, "NUM") == 0) {
        printParserLog("A valid ID detected, factor definition success");
        return 1;
    } else if(strcmp(currToken, "(") == 0) {
        printParserLog("Checking for (expression)");
        getNextToken();
        if (expr() == 1) {
            printParserLog("Expression detected");
            if(strcmp(currToken, ")") == 0) {
                printParserLog("A valid (expression) detected, factor definition success");
            } else {
                printParserLog("Invalid Factor definition, an expression should be ecnlosed within ()");
                addErrorLog("Invalid Factor definition, an expression should be ecnlosed within ()", currLineNo);
                return 0;
            }
        }
        
    } else if (strcmp(currTokenType, "NOT") == 0) {
        printParserLog("Detected NOT, checking for factor");
        getNextToken();
        if (factor() == 1) {
            printParserLog("A valid factor definition found");
            return 1;
        } else {
            printParserLog("Invalid Factor definition, a NOT should be followed by a factor");
            addErrorLog("Invalid Factor definition, a NOT should be followed by a factor", currLineNo);
            return 0;
        }
    } else if (sign() == 1) {
        printParserLog("Detected sign, checking for factor");
        getNextToken();
        if (factor() == 1) {
            printParserLog("A valid factor definition found");
            return 1;
        } else {
            printParserLog("Invalid Factor definition, a sign should be followed by a factor");
            addErrorLog("Invalid Factor definition, a sign should be followed by a factor", currLineNo);
            return 0;
        }
        
    }
    return 1;
}

int term() {
    if(factor() == 1) {
        printParserLog("Detected factor, checking for mult operator");
        getNextToken();
        if(multOp() == 1) {
            printParserLog("Multiplication operation detected");
            getNextToken();
            printParserLog("Checking for a factor");
            if(factor() == 1) {
                printParserLog("Factor detected");
                return 1;
            } else {
                printParserLog("A multiplication operator should be followed by a factor for a term");
                addErrorLog("Invalid term declaration, a multiplication operator should be followed by a factor for a term", currLineNo);
                return 0;
            }
        } else {
            return 1;
        }
    } else {
        printParserLog("Invalid term declaration");
        addErrorLog("Invalid term declaration found, expected a term or a factor", currLineNo);
        return 0;
    }
}

int arithExpr() {
    printParserLog("Checking arithmatic expression");
    if(addOp() == 1) {
        printParserLog("Add operation detected inside arithmatic expression");
        getNextToken();
        if(term() == 1) {
            printParserLog("Detected term after add operation");
            return 1;
        } else {
            printParserLog("Invalid arithmatic expression detected, expected a term after add operation");
            addErrorLog("Invalid arithmatic expression detected, expected a term after add operation", currLineNo);
            return 0;
        }
    } else if(term() == 1) {
        printParserLog("Detected term");
        return 1;
    } else {
        printParserLog("No arithmatic definitions found");
        return 0;
    }
}

int expr() {
    printParserLog("Checking for expression defintion");
    if (arithExpr() == 1) {
        printParserLog("Arithmatic definition detected, checking for relational operator");
        if (relOp() == 1) {
            printParserLog("Relational operator detected, checking for arithmatic definition");
            getNextToken();
            if(arithExpr() == 1) {
                printParserLog("Expression defined successfully");
                return 1;
            } else {
                printParserLog("Invalid expression definition, a relational operator should be followed by an arithmatic operator");
                addErrorLog("Invalid expression definition, a relational operator should be followed by an arithmatic operator", currLineNo);
                return 0;
            }
        } else { 
            // no need to call getNextToken() after this method
            printParserLog("Expression defintion successful");
            return 1;
        }
    } else {
        printParserLog("Invalid expression definition, should start with an arithmatic expression");
        addErrorLog("Invalid expression definition, should start with an arithmatic expression", currLineNo);
        return 0;
    }
    
    return 1;
}

int statBlock() {
    printParserLog("Checking for statBlock defintion");
    printf("INITIAL STATBLOCK TOKEN: %s\n", currToken);
    if(strcmp(currToken, "{") == 0) {
        printParserLog("Detected { , checking for statement");
        getNextToken();
        if(statement() == 1) {
            printParserLog("Detected statement , checking for more statements");
            while (statement() == 1){
                printParserLog("Detected more statements");
                getNextToken();
            }
        } else {
            printParserLog("No statements found inside statBlock");
        }
        if(strcmp(currToken, "}") == 0) {
            printParserLog("statBlock detected succesfully");
            return 1;
        } else {
            printParserLog("Invalid statBlock definition, expected a } at the end");
            addErrorLog("Invalid statBlock definition, expected a } at the end", currLineNo);
            return 0;
        }
    } else if(statement() == 1){
        printParserLog("statBlock detected succesfully");
        return 1;
    } else {
        printParserLog("Empty statBlock detected without {}");
        return 1;
    }
}

int statement() {
    printParserLog("Checking for statement defintion");
    printf("TOKEN INSIDE STMT: %s\n", currToken);
    if(variable() == 1) {
        printParserLog("Variable detected inside statement, checking for assignment operator");
        getNextToken();
        if(assignOp() == 1) {
            printParserLog("Assign operator detected, checking for expression");
            getNextToken();
            if(expr() == 1) {
                printParserLog("Expression detected, checking for ;");
                getNextToken();
                if(strcmp(currToken, ";") == 0) {
                    printParserLog("Statement definition succesfull");
                    return 1;
                } else {
                    printParserLog("Invalid statement definition, a ; expected after expression");
                    addErrorLog("Invalid statement definition, a ; expected after expression", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid statement definition, a valid expression detected after assignment operator");
                addErrorLog("Invalid statement definition, a valid expression detected after assignment operator", currLineNo);
                return 0;
            }
        }
    } else if (strcmp(currToken, "if") == 0) {
        printParserLog("If condition detected, checking for (expression)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            if(expr() == 1) {
                printParserLog("Detected expression , checking for )");
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for then");
                    getNextToken();
                    if(strcmp(currToken, "then") == 0) {
                        printParserLog("Detected then , checking for statBlock");
                        getNextToken();
                        if(statBlock() == 1) {
                            printParserLog("Detected statBlock , checking for else");
                            printf("ELSE CHECK TOKEN: %s\n", currToken);
                            // getNextToken();
                            if(strcmp(currToken, "else") == 0) {
                                printParserLog("Detected else , checking for statBlock");
                                getNextToken();
                                printf("AFTER ELSE TOKEN: %s\n", currToken);
                                if(statBlock() == 1) {
                                    printParserLog("Detected statBlock , checking for ;");
                                    getNextToken();
                                    if(strcmp(currToken, ";") == 0) {
                                        printParserLog("If condition definition succesfull");
                                        return 1;
                                    } else {
                                        printParserLog("Invalid if condition definition, a ; expected at the end");
                                        addErrorLog("Invalid if condition definition, a ; expected at the end", currLineNo);
                                        return 0;
                                    }
                                } else {
                                    printParserLog("Invalid if condition definition, a else should be followed by an statBlock");
                                    addErrorLog("Invalid if condition definition, a else should be followed by an statBlock", currLineNo);
                                    return 0;
                                }
                            } else {
                                printParserLog("Invalid if condition definition, a statBlock should be followed by an else");
                                addErrorLog("Invalid if condition definition, a statBlock should be followed by an else", currLineNo);
                                return 0;
                            }
                        } else {
                            printParserLog("Invalid if condition definition, a statBlock expected after then");
                            addErrorLog("Invalid if condition definition, a statBlock expected after then", currLineNo);
                            return 0;
                        }
                    } else {
                        printParserLog("Invalid if condition definition, an expression should be followed by then");
                        addErrorLog("Invalid if condition definition, an expression should be followed by then", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Invalid if condition definition, an expression should be followed by a )");
                    addErrorLog("Invalid if condition definition, an expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid if condition definition, a ( should be followed by a valid expression");
                addErrorLog("Invalid if condition definition, a ( should be followed by a valid expression", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Invalid if condition definition, if should be followed with (expression)");
            addErrorLog("Invalid if condition definition, if should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "while") == 0) {
        printParserLog("While condition detected, checking for (expression)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            if(expr() == 1) {
                printParserLog("Detected expression , checking for )");
                getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for do");
                    getNextToken();
                    if(strcmp(currToken, "do") == 0) {
                        printParserLog("Detected else , checking for statBlock");
                        getNextToken();
                        if(statBlock() == 1) {
                            printParserLog("Detected statBlock , checking for ;");
                            getNextToken();
                            if(strcmp(currToken, ";") == 0) {
                                printParserLog("while condition definition succesfull");
                                return 1;
                            } else {
                                printParserLog("Invalid while condition definition, a ; expected at the end");
                                addErrorLog("Invalid while condition definition, a ; expected at the end", currLineNo);
                                return 0;
                            }
                        } else {
                            printParserLog("Invalid while condition definition, a do should be followed by an statBlock");
                            addErrorLog("Invalid while condition definition, a do should be followed by an statBlock", currLineNo);
                            return 0;
                        }
                    } else {
                        printParserLog("Invalid while condition definition, a statBlock should be followed by a do");
                        addErrorLog("Invalid while condition definition, a statBlock should be followed by a do", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Invalid while condition definition, an expression should be followed by a )");
                    addErrorLog("Invalid while condition definition, an expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid while condition definition, a ( should be followed by a valid expression");
                addErrorLog("Invalid while condition definition, a ( should be followed by a valid expression", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Invalid while condition definition, while should be followed with (expression)");
            addErrorLog("Invalid while condition definition, while should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "read") == 0) {
        printParserLog("Read detected, checking for (variable)");
        getNextToken();
        if(strcmp(currToken, "(" == 0)) {
            printParserLog("Detected ( , checking for variable");
            getNextToken();
            if(variable() == 1) {
                printParserLog("Detected variable , checking for )");
                getNextToken();
                if(strcmp(currToken, ")" == 0)) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";" == 0)) {
                        printParserLog("Read definition succesfull");
                        return 1;
                    } else {
                        printParserLog("Invalid read definition, a ; expected at the end");
                        addErrorLog("Invalid read definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Invalid read definition, a variable should be followed by a )");
                    addErrorLog("Invalid read definition, a variable should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid read definition, ( should be followed by a variable");
                addErrorLog("Invalid read definition, ( should be followed by a variable", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Invalid read definition, read should be followed with (variable)");
            addErrorLog("Invalid read definition, read should be followed with (variable)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "write") == 0) {
        printParserLog("Write detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            if(expr() == 1) {
                printParserLog("Detected expression , checking for )");
                getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        printParserLog("Write definition succesfull");
                        return 1;
                    } else {
                        printParserLog("Invalid write definition, a ; expected at the end");
                        addErrorLog("Invalid write definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Invalid write definition, a expression should be followed by a )");
                    addErrorLog("Invalid write definition, a expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid write definition, ( should be followed by a expression");
                addErrorLog("Invalid write definition, ( should be followed by a expression", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Invalid write definition, write should be followed with (expression)");
            addErrorLog("Invalid write definition, write should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "return") == 0) {
        printParserLog("Return detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            if(expr() == 1) {
                printParserLog("Detected expression , checking for )");
                getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        printParserLog("Return definition succesfull");
                        return 1;
                    } else {
                        printParserLog("Invalid return definition, a ; expected at the end");
                        addErrorLog("Invalid return definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Invalid return definition, a expression should be followed by a )");
                    addErrorLog("Invalid return definition, a expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                printParserLog("Invalid return definition, ( should be followed by a expression");
                addErrorLog("Invalid return definition, ( should be followed by a expression", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Invalid statement definition, expected variable/if/while/read/write/return");
            addErrorLog("Invalid statement definition, expected variable/if/while/read/write/return", currLineNo);
            return 0;
        }
    } else {
        printParserLog("No statements found");
        return 0;
    }
}

int arraySize() {
    if (strcmp(currToken, "[") == 0) {
        printParserLog("A [ detected, checking for Digit");
        getNextToken();
        if (strcmp(currTokenType, "DIGIT") == 0) {
            printParserLog("A Digit detected, checking for ]");
            getNextToken();
            if (strcmp(currToken, "]") == 0) {
                printParserLog("Array definition success");
                return 1;
            } else {
                printParserLog("Invalid array size defintion, ] expected after array size");
                addErrorLog("Invalid array size defintion, ] expected after array size", currLineNo);
                return 0;
            }                        
        } else {
            printParserLog("Invalid array size defintion, expected a digit or int inside []");
            return 0;
        }
    } else {
        printParserLog("Invalid array size defintion, expected a [digit/int]");
        return 0;
    }
}

int aParams() {
    if(expr() == 1) {
        printParserLog("A valid expression found, checking for more aParams");
        getNextToken();
        while(strcmp(currToken, ",") != 0) {
            if(aParams() == 1) {
                printParserLog("More aParams found");
                getNextToken();
            } else {
                printParserLog("Invalid aParams definition, a comman should be followed by another aParam");
                addErrorLog("Invalid aParams definition, a comman should be followed by another aParam", currLineNo);
                return 0;
            }
        }
    } else {
        printParserLog("Empty aParams found");
        return 1;
    }
}

int fParams() {
    if (type() == 1) {
        printParserLog("A valid type detected, checking for ID");
        getNextToken();
        if (strcmp(currTokenType, "ID") == 0) {
            printParserLog("A valid ID detected, checking for array sizes [int]");
            getNextToken();
            if(arraySize() == 1) {
                printParserLog("A valid array size detected, checking for more array sizes ");
                getNextToken();
                while(strcmp(currToken, ",") == 0) {
                    // fParamsTail
                    getNextToken();
                    if(fParams() == 1) {
                        // continue
                        return 1;
                    } else {
                        // printf("TOKEN: %s\n", currToken);
                        printParserLog("No more fParams detected");
                        // addErrorLog("Invalid fParams definition, a comma should be followed by another valid fParam", currLineNo);
                        return 0;
                    }
                }
            } else {
                printParserLog("No array size or fParamsTail detected");
                return 1;
            }
        } else {
            printParserLog("Invalid fParam defintion, type should be followed with an ID...");
            addErrorLog("Invalid fParam defintion, type should be followed with an ID", currLineNo);
            return 0;
        }
    } else {
        printParserLog("Invalid fParam defintion, type is expected...");
        addErrorLog("Invalid fParam defintion, type is expected", currLineNo);
        return 0;
    }
}

int funcBody() {
    printParserLog("Came inside function body");
    getNextToken();

    if (strcmp(currToken, "{") != 0) {
        addErrorLog("Invalid function body definition, should start with {", currLineNo);    
    }
    getNextToken();
    
    while (strcmp(currToken, "}") != 0) {
        if (type() == 1) {
            printParserLog("Checking for variable definitions");
            char name[10];
            char type[10];
            strcpy(type, currToken);
            getNextToken();  
            if (strcmp(currTokenType, "ID") == 0) {
                printParserLog("Valid ID detected...");
                strcpy(name, currToken);
                getNextToken();
                if (strcmp(currToken, "[") == 0) {
                    printParserLog("Variable declration detected");
                    variableSym variable;
                    strcpy(variable.name, name);
                    strcpy(variable.type, type);
                    varDecl(variable);
                }
            } else {
                printParserLog("Invalid variable declaration: a variable name is not specified!");
                addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Type check failed, checking for statement definition");
            printf("CHECK STMT TOKEN: %s\n", currToken);
            if(statement() == 1) {
                printParserLog("A valid statement detected, checking for more statements");
                getNextToken();
                while(statement() == 1) {
                    printParserLog("More statements detected");
                    getNextToken();
                }
            } else {
                printf("FAILED STMT TOKEN: %s\n", currToken);
                printParserLog("Statement detection failed");
            }
        }
        getNextToken();
    }
    return 1;
}

int funcDef(functionSym function){
    // current token is (
    printParserLog("Came inside function defintion");

    if(updateFunctionSymbolTable(function, currLineNo) == 0) {
        printParserLog("Update function symbol table failed...");
        return 0;
    }

    printParserLog("Function symbol table update succesfully");

    getNextToken();

    // fParams
    while (strcmp(currToken, ")") != 0) {
        printParserLog("Checking for fParams");
        if(fParams() == 1) {

        } else {
            printParserLog("No fParams detected, so checking the funcBody next");
        }
    }

    if(funcBody() == 1) {

    } else {
        // throw error
    }

    return 1;
}

int classDecl() {
    classSym classSym;
    getNextToken();      
    printParserLog("Came inside class declaration...");  

    if (strcmp(currTokenType, "ID") == 0) {
        
        strcpy(classSym.name, currToken);
        getNextToken();
        printParserLog("Detected ID..."); 

        if (strcmp(currToken, "{") == 0) {
            getNextToken();
            printParserLog("Came inside class body...");
            updateClassSymbolTable(classSym, currLineNo);

            int x=0;
            while (strcmp(currToken, "}") != 0) {
                if (type() == 1) {
                    char name[10];
                    char type[10];
                    strcpy(type, currToken);
                    getNextToken();  

                    if (strcmp(currTokenType, "ID") == 0) {
                        printParserLog("Valid ID detected...");
                        strcpy(name, currToken);
                        getNextToken();

                        if (strcmp(currToken, "[") == 0) {
                            printParserLog("Variable declration detected");
                            variableSym variable;
                            strcpy(variable.name, name);
                            strcpy(variable.type, type);
                            varDecl(variable);
                        } else if(strcmp(currToken, "(") == 0) {
                            printParserLog("Function declration detected");
                            functionSym function;
                            strcpy(function.name, name);
                            strcpy(function.type, type);
                            funcDef(function);
                        }
                        getNextToken();
                    } else {
                        addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                        return 0;
                    }
                } else {
                    // printf("TOKEN: %s\n", currToken);
                    printParserLog("Type check failed...");
                    return 0;
                }
            }
            return 1;
             
        } else {
            addErrorLog("Class declaration should start with {", currLineNo);
            return 0;
        }
    } else {
        addErrorLog("A class should be followed by a class name!", currLineNo);
        return 0;
    }
    
}

void parse() {

    printPhase("Syntax, Semantic Analysis");

    getNextToken();

    while (strcmp(currToken, "exit") != 0) {
        if (strcmp(currToken, "class") == 0) {
            printParserLog("Found class declaration...");
            if (classDecl() == 1) {
                // to-do
            } else {
                printParserLog("Error occured in class declaration");
            }
        } else {
            addErrorLog("Program should start with class defintion!", currLineNo);
        }
        getNextToken();
    }
}

void parseStart() {
    initFile();
    parse();

    printAllSymbolTables();
    printErrorLogs();
}