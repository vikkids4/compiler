#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "symbolTable.h"
#include "codeGen.h"
#include "AST.h"

FILE *tokensFilePtr;
char line[255];
char* token;
char* currToken;
char* currTokenType;
char* currLineNo;
int scope = 0;
OneOperand oneOperand;
TwoOperand twoOperand;

void initTokenFile() {
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

char* varTwo;

// StatementNode *stmtNodeToPtr(StatementNode statementNode) {
//     StatementNode *stmtNodePtr;
//     stmtNodePtr->init = statementNode.init;
//     stmtNodePtr->ifElseStmtNode = statementNode.ifElseStmtNode;
//     stmtNodePtr->readStmtNode = statementNode.readStmtNode;
//     stmtNodePtr->returnStmtNode = statementNode.returnStmtNode;
//     stmtNodePtr->statementType = statementNode.statementType;
//     stmtNodePtr->varAssignStmt = statementNode.varAssignStmt;
//     stmtNodePtr->whileStmtNode = statementNode.whileStmtNode;
//     stmtNodePtr->writeStmtNode = statementNode.writeStmtNode;
//     return stmtNodePtr;
// }

// VariableNode *varNodeToPtr(VariableNode *variableNode) {
//     VariableNode *varNodePtr;
//     strcpy(varNodePtr->id , variableNode->id);
//     varNodePtr->idnestCount = variableNode->idnestCount;
//     memcpy(varNodePtr->idnestNode , variableNode->idnestNode, sizeof *varNodePtr->idnestNode);
//     varNodePtr->indiceCount = variableNode->indiceCount;
//     memcpy(varNodePtr->indiceNode , variableNode->indiceNode, sizeof *varNodePtr->indiceNode);
    
//     varNodePtr->init = variableNode->init;
//     return varNodePtr;
// }

int type() {
    if (strcmp(currToken, "integer") == 0 || 
        strcmp(currToken, "real") == 0    || 
        strcmp(currToken, "id") == 0) {
        printParserLog("Valid type detected...");
        return 1;
    } else {
        printParserLog("Invalid type detected");
        return 0;
    }
}

int varDecl(variableSym variable, VarDeclNode *varDeclNode) {
    if (strcmp(currToken, "[") == 0) {
        getNextToken();
        if (strcmp(currTokenType, "DIGIT") == 0   || 
            strcmp(currTokenType, "INTEGER") == 0 || 
            strcmp(currTokenType, "NUM") == 0) {
            
            strcpy(variable.size, currToken);
            getNextToken(); 
            if (strcmp(currToken, "]") == 0) {
                getNextToken();
                // if (strcmp(currToken, "=") == 0) {                
                //     getNextToken();
                //     if(strcmp(currTokenType, "ID") == 0 ||
                //         strcmp(currTokenType, "INTEGER") == 0 ||
                //         strcmp(currTokenType, "NONZERO") == 0 ||
                //         strcmp(currTokenType, "ALPHANUM") == 0 ||
                //         strcmp(currTokenType, "FRACTION") == 0 ||
                //         strcmp(currTokenType, "DIGIT") == 0) {
                //         strcpy(variable.value, currToken);
                //     } else {
                //         printParserLog("Invalid variable declaration: check variable assignment");
                //         addErrorLog("Invalid variable declaration: check variable assignment", currLineNo);
                //         return 0;
                //     }
                //     getNextToken();
                // }
                if (strcmp(currToken, ";") == 0) {
                    // insert into variable symbol table
                    printParserLog("Variable declaration successful!");
                    strcpy(variable.value, "null");
                    if(updateVariableSymbolTable(variable, currLineNo) == 0) {
                        printParserLog("Update variable symbol table failed...");
                        return 0;
                    }
                    return 1;
                } else {
                    addErrorLog("Invalid variable declaration: expected a ; at the end", currLineNo);
                    return 0;
                }
            } else {
                addErrorLog("Invalid variable declaration: expected a ] after variable size", currLineNo);
                return 0;
            }
        }
    } else if (strcmp(currToken, ";") == 0) {
        // insert into variable symbol table
        printParserLog("Variable declaration successful!");
        strcpy(variable.size, "10");
        strcpy(variable.value, "null");
        if(updateVariableSymbolTable(variable, currLineNo) == 0) {
            printParserLog("Update variable symbol table failed...");
            return 0;
        }
        return 1;
    } else {
        addErrorLog("Invalid variable declaration: expected a ; at the end", currLineNo);
        return 0;
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
        ArithExprNode arithExprNode0;
        arithExprNode0.init = false;
        if (arithExpr(&arithExprNode0) == 1) {
            arithExprNode0.init = false;
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

int variable(VariableNode *variableNode) {
    printParserLog("Checking for a variable");
    // at this point ID and next token would have been fetched
    if(strcmp(currTokenType, "ID") == 0) {
        strcpy(variableNode->id,currToken);
        // adding this inside because Idnest also needs ID first so saving some logic space
        // if(idnest() == 1) {
        //     printParserLog("Valid idnest detected, checking for more idnest definition if available");
        //     while(idnest() == 1) {
        //         printParserLog("More idnest definition found");
        //         getNextToken();
        //     }
        // } 

        strcpy(twoOperand.opcode, "MOV");
        // printParserLog("Valid ID detected inside variable, checking for indice");
        // if(indice() == 1) {
        //     printParserLog("Valid indice detected, checking for more indice definition if available");
        //     getNextToken();
        //     while (indice() == 1) {
        //         printParserLog("Looping indices inside idnest");
        //         getNextToken();
        //     }
        //     // no need to getNextToken() after this method
        //     return 1;
        // }
        return 1;
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

int factor(FactorNode *factorNode) {
    VariableNode variableNode;
    variableNode.init = false;
    memset(variableNode.idnestNode, 0, sizeof(variableNode.idnestNode));
    memset(variableNode.indiceNode, 0, sizeof(variableNode.indiceNode));
    variableNode.idnestCount = 0;
    variableNode.indiceCount = 0;

    IdnestNode idnestNode;
    idnestNode.init = false;
    memset(idnestNode.indiceNode, 0, sizeof(idnestNode.indiceNode));
    idnestNode.indiceCount = 0;
    if (variable(&variableNode) == 1) {
        factorNode->variable = &variableNode;
        // no need to getNextToken() afer this
        printParserLog("Valid variable definition found, factor defintion success");
        return 1;
    } else if (idnest(&idnestNode) == 1) {
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
        ExprNode exprNode;
        exprNode.init = false;
        if (expr(&exprNode) == 1) {
            exprNode.init = true;
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
        FactorNode factorNode;
        factorNode.init = true;
        if (factor(&factorNode) == 1) {
            factorNode.init = false;
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
        FactorNode factorNode;
        factorNode.init = true;
        if (factor(&factorNode) == 1) {
            factorNode.init = false;
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

int term(TermNode *termNode) {
    FactorNode factorNode;
    factorNode.init = false;
    if(factor(&factorNode) == 1) {
        factorNode.init = true;
        termNode->factor = factorNode;
        printParserLog("Detected factor, checking for mult operator");
        getNextToken();
        if(multOp() == 1) {
            printParserLog("Multiplication operation detected");
            getNextToken();
            printParserLog("Checking for a factor");
            if(factor(&factorNode) == 1) {
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

int arithExpr(ArithExprNode *arithExprNode) {
    printParserLog("Checking arithmatic expression");
    TermNode termNode;
    termNode.init = false;
    if(term(&termNode) == 1) {
        termNode.init = false;
        arithExprNode->term = termNode;
        printParserLog("Detected term");
        return 1;
    } else if(addOp() == 1) {
        printParserLog("Add operation detected inside arithmatic expression");
        getNextToken();
        if(term(&termNode) == 1) {
            termNode.init = true;
            printParserLog("Detected term after add operation");
            return 1;
        } else {
            printParserLog("Invalid arithmatic expression detected, expected a term after add operation");
            addErrorLog("Invalid arithmatic expression detected, expected a term after add operation", currLineNo);
            return 0;
        }
    } else {
        printParserLog("No arithmatic definitions found");
        return 0;
    }
}

int expr(ExprNode *exprNode) {
    printParserLog("Checking for expression defintion");
    ArithExprNode arithExprNode;
    arithExprNode.init = false;
    if (arithExpr(&arithExprNode) == 1) {
        // arithExprNode.init = true;
        exprNode->arithExpr = arithExprNode;
        RelArithExprNode relArithExprNode;
        relArithExprNode.init = false;
        printParserLog("Arithmatic definition detected, checking for relational operator");
        if (relOp() == 1) {
            strcpy(relArithExprNode.relOp, currToken);
            printParserLog("Relational operator detected, checking for arithmatic definition");
            getNextToken();
            arithExprNode.init = false;
            if(arithExpr(&arithExprNode) == 1) {
                arithExprNode.init = true;
                relArithExprNode.arithExpr2 = arithExprNode;
                relArithExprNode.init = true;
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

int statBlock(StatBlockNode *statBlockNode) {
    printParserLog("Checking for statBlock defintion");
    StatementNode statementNode;
    statementNode.init = false;
    if(strcmp(currToken, "{") == 0) {
        printParserLog("Detected { , checking for statement");
        getNextToken();
        if(statement(&statementNode) == 1) {
            statementNode.init = true;
            StatementNode *statementNodePtr4 = stmtNodeToPtr(statementNode);
            statBlockNode->statement[++statBlockNode->statementNodeCount] = statementNodePtr4;
            printParserLog("Detected statement , checking for more statements");
            getNextToken();
            while (statement(&statementNode) == 1){
                StatementNode *statementNodePtr4 = stmtNodeToPtr(statementNode);
                statBlockNode->statement[++statBlockNode->statementNodeCount] = statementNodePtr4;
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
    } else if(statement(&statementNode) == 1){
        StatementNode *statementNodePtr4 = stmtNodeToPtr(statementNode);
        statBlockNode->statement[++statBlockNode->statementNodeCount] = statementNodePtr4;
        printParserLog("statBlock detected succesfully");
        return 1;
    } else {
        printParserLog("Empty statBlock detected without {}");
        return 1;
    }
}

int statement(StatementNode *statementNode) {
    printParserLog("Checking for statement defintion");
    VariableNode variableNode0;
    variableNode0.init = false;
    memset(variableNode0.idnestNode, 0, sizeof(variableNode0.idnestNode));
    memset(variableNode0.indiceNode, 0, sizeof(variableNode0.indiceNode));
    variableNode0.idnestCount = 0;
    variableNode0.indiceCount = 0;
    if(variable(&variableNode0) == 1) {
        statementNode->statementType = 0;
        variableNode0.init = true;
        VarAssignStmtNode varAssignStmtNode;
        varAssignStmtNode.init = false;
        varAssignStmtNode.variable = variableNode0;

        printParserLog("Variable detected inside statement, checking for assignment operator");
        getNextToken();
        if(assignOp() == 1) {
            printParserLog("Assign operator detected, checking for expression");
            strcpy(twoOperand.opcode, "MOV"); // the operand1 will be degines inside variable()
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                varAssignStmtNode.expr = exprNode;
                varAssignStmtNode.init = true;
                printParserLog("Expression detected, checking for ;");
                // getNextToken();
                // getNextToken();
                if(strcmp(currToken, ";") == 0) {
                    statementNode->varAssignStmt = varAssignStmtNode;
                    statementNode->init = true;
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
        statementNode->statementType = 1;
        printParserLog("If condition detected, checking for (expression)");
        getNextToken();
        IfElseStmtNode ifElseStmtNode;
        ifElseStmtNode.init = false;
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                ifElseStmtNode.expr = exprNode;
                printParserLog("Detected expression , checking for )");
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for then");
                    getNextToken();
                    if(strcmp(currToken, "then") == 0) {
                        printParserLog("Detected then , checking for statBlock");
                        getNextToken();
                        StatBlockNode statBlockNode;
                        statBlockNode.init = false;
                        memset(statBlockNode.statement, 0, sizeof(statBlockNode.statement));
                        statBlockNode.statementNodeCount = 0;
                        if(statBlock(&statBlockNode) == 1) {
                            statBlockNode.init = true;
                            ifElseStmtNode.statBlock1 = statBlockNode;
                            printParserLog("Detected statBlock , checking for else");
                            getNextToken();
                            if(strcmp(currToken, "else") == 0) {
                                printParserLog("Detected else , checking for statBlock");
                                getNextToken();
                                statBlockNode.init = false;
                                if(statBlock(&statBlockNode) == 1) {
                                    statBlockNode.init = true;
                                    ifElseStmtNode.statBlock2 = statBlockNode;
                                    ifElseStmtNode.init = true;
                                    printParserLog("Detected statBlock , checking for ;");
                                    getNextToken();
                                    if(strcmp(currToken, ";") == 0) {
                                        statementNode->ifElseStmtNode = ifElseStmtNode;
                                        statementNode->init = true;
                                        printParserLog("If condition definition succesfull");
                                        getNextToken();
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
        statementNode->statementType = 2;
        WhileStmtNode whileStmtNode;
        whileStmtNode.init = false;
        printParserLog("While condition detected, checking for (expression)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                whileStmtNode.expr = exprNode;
                printParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for do");
                    getNextToken();
                    if(strcmp(currToken, "do") == 0) {
                        printParserLog("Detected else , checking for statBlock");
                        getNextToken();
                        StatBlockNode statBlockNode;
                        statBlockNode.init = false;
                        memset(statBlockNode.statement, 0, sizeof(statBlockNode.statement));
                        statBlockNode.statementNodeCount = 0;
                        if(statBlock(&statBlockNode) == 1) {
                            statBlockNode.init = true;
                            whileStmtNode.statBlock = statBlockNode;
                            whileStmtNode.init = true;
                            printParserLog("Detected statBlock , checking for ;");
                            getNextToken();
                            if(strcmp(currToken, ";") == 0) {
                                statementNode->whileStmtNode = whileStmtNode;
                                statementNode->init = true;
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
        statementNode->statementType = 3;
        ReadStmtNode readStmtNode;
        readStmtNode.init = true;
        printParserLog("Read detected, checking for (variable)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for variable");
            getNextToken();
            VariableNode variableNode1;
            variableNode1.init = false;
            memset(variableNode1.idnestNode, 0, sizeof(variableNode1.idnestNode));
            memset(variableNode1.indiceNode, 0, sizeof(variableNode1.indiceNode));
            variableNode1.idnestCount = 0;
            variableNode1.indiceCount = 0;
            if(variable(&variableNode1) == 1) {
                variableNode1.init = true;
                readStmtNode.variable = variableNode1;
                readStmtNode.init = true;
                printParserLog("Detected variable , checking for )");
                getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->readStmtNode = readStmtNode;
                        statementNode->init = true;
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
        statementNode->statementType = 4;
        WriteStmtNode writeStmtNode;
        writeStmtNode.init = false;
        printParserLog("Write detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                writeStmtNode.expr = exprNode;
                writeStmtNode.init = true;
                printParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->writeStmtNode = writeStmtNode;
                        statementNode->init = true;
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
        statementNode->statementType = 5;
        ReturnStmtNode returnStmtNode;
        returnStmtNode.init = false;
        printParserLog("Return detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            printParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                returnStmtNode.expr = exprNode;
                returnStmtNode.init = true;
                printParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    printParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->returnStmtNode = returnStmtNode;
                        statementNode->init = true;
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
    ExprNode exprNode;
    exprNode.init = false;
    if(expr(&exprNode) == 1) {
        exprNode.init = true;
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
        addErrorLog("Invalid type definition, only the types integer, real are allowed", currLineNo);
        addErrorLog("Invalid fParam defintion, type is expected", currLineNo);
        return 0;
    }
}


int funcBody(FuncBodyNode *funcBodyNode) {
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
                if (strcmp(currToken, "[") == 0 || strcmp(currToken, "=") == 0 || strcmp(currToken, ";") == 0) {
                    printParserLog("Variable declration detected");
                    VarDeclNode varDeclNode;
                    varDeclNode.init = false;
                    memset(varDeclNode.arraySize, 0, sizeof(varDeclNode.arraySize));
                    varDeclNode.arraySizeCount = 0;
                    strcpy(varDeclNode.id, name);
                    strcpy(varDeclNode.type, type);
                    variableSym variable;
                    strcpy(variable.name, name);
                    strcpy(variable.type, type);
                    varDecl(variable, &varDeclNode);
                    varDeclNode.init = true;
                    funcBodyNode->varDecl[++funcBodyNode->varCount] = varDeclNode;
                }
            } else {
                printParserLog("Invalid variable declaration: a variable name is not specified!");
                addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                return 0;
            }
        } else {
            printParserLog("Type check failed, checking for statement definition");
            StatementNode statementNode;
            statementNode.init = false;
            if(statement(&statementNode) == 1) {
                statementNode.init = true;
                StatementNode *statementNodePtr1 = &statementNode;
                funcBodyNode->statement[++funcBodyNode->statementCount] = statementNodePtr1;
                printParserLog("A valid statement detected, checking for more statements");
                getNextToken();
                while(statement(&statementNode) == 1) {
                    StatementNode *statementNodePtr0 = stmtNodeToPtr(statementNode);
                    funcBodyNode->statement[++funcBodyNode->statementCount] = statementNodePtr0;
                    printParserLog("More statements detected");
                    getNextToken();
                }
            } else {
                printParserLog("Statement detection failed");
            }
        }
        if(strcmp(currToken, "}") != 0) {
            getNextToken();
        }
    }
    funcBodyNode->init = true;
    return 1;
}

int funcDef(functionSym function, FuncDefNode *funcDefNode){
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

    FuncBodyNode funcBodyNode;
    funcBodyNode.init = false;
    memset(funcBodyNode.varDecl, 0, sizeof(funcBodyNode.varDecl));
    memset(funcBodyNode.statement, 0, sizeof(funcBodyNode.statement));
    funcBodyNode.varCount = 0;
    funcBodyNode.statementCount = 0;
    if(funcBody(&funcBodyNode) == 1) {
        funcBodyNode.init = true;
        funcDefNode->funcBody = funcBodyNode;
        printParserLog("Function body definition successful, checking for ;");
        getNextToken();
        if(strcmp(currToken, ";") == 0) {
            printParserLog("Function definition successful");
            return 1;
        } else {
            printParserLog("Invalid function definition, expected ; at the end");
            addErrorLog("Invalid function definition, expected ; at the end", currLineNo);
        }
    } else {
        printParserLog("Invalid function body definition");
        addErrorLog("Invalid function body definition", currLineNo);
        return 0;
    }
    return 1;
}

int classDecl(ClassDeclNode *classDeclNode) {
    classSym classSym;
    getNextToken();      
    printParserLog("Came inside class declaration..."); 
    if (strcmp(currTokenType, "ID") == 0) {
        strcpy(classDeclNode->id, currToken);
        strcpy(classSym.name, currToken);
        strcpy(oneOperand.opcode, "CALL");
        strcpy(oneOperand.operand, classSym.name);
        generateOneOperandCode(oneOperand, 0); // class decl - scope 0
        scope++; 
        generateLabelCode(classSym.name,scope);
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
                        if (strcmp(currToken, "[") == 0 || strcmp(currToken, "=") == 0 || strcmp(currToken, ";") == 0) {
                            printParserLog("Variable declration detected");
                            VarDeclNode varDeclNode;
                            varDeclNode.init = false;
                            memset(varDeclNode.arraySize, 0, sizeof(varDeclNode.arraySize));
                            varDeclNode.arraySizeCount = 0;
                            strcpy(varDeclNode.id, name);
                            strcpy(varDeclNode.type, type);
                            variableSym variable;
                            strcpy(variable.name, name);
                            strcpy(variable.type, type);
                            varDecl(variable, &varDeclNode);
                            varDeclNode.init = true;
                            classDeclNode->varDecl[++classDeclNode->varDeclCount] = varDeclNode;
                        } else if(strcmp(currToken, "(") == 0) {
                            // scope++;
                            printParserLog("Function declration detected");
                            FuncDefNode funcDefNode;
                            funcDefNode.init = false;
                            FuncHeadNode funcHeadNode;
                            strcpy(funcHeadNode.id, name);
                            strcpy(funcHeadNode.type, type);
                            funcHeadNode.init = true;
                            funcDefNode.funcHead = funcHeadNode;

                            functionSym function;
                            strcpy(function.name, name);
                            strcpy(function.type, type);
                            strcpy(oneOperand.opcode, "CALL");
                            strcpy(oneOperand.operand, name);
                            generateOneOperandCode(oneOperand, scope);
                            
                            generateLabelCode(function.name, scope);

                            funcDef(function, &funcDefNode);
                            funcDefNode.init = true;
                            classDeclNode->funcDef[++classDeclNode->funcDefCount] = funcDefNode;
                            scope--;
                        }
                        getNextToken();
                    } else {
                        addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                        return 0;
                    }
                } else {
                    printParserLog("Type check failed...");
                    addErrorLog("Invalid type definition, only the types integer, real are allowed", currLineNo);
                    return 0;
                }
            }
            if (scope != 0) {
                scope--;
                // if the scope is 0 then print the assembly code
                // clear the array of scopes
                // get ready for next set of scopes
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

void parse(ProgNode *progNode) {
    printPhase("Syntax, Semantic Analysis");
    getNextToken();
    while (strcmp(currToken, "exit") != 0) {
        if (strcmp(currToken, "class") == 0) {
            printParserLog("Found class declaration...");
            ClassDeclNode classDeclNode;
            classDeclNode.init = false;
            memset(classDeclNode.varDecl, 0, sizeof(classDeclNode.varDecl));
            memset(classDeclNode.funcDef, 0, sizeof(classDeclNode.funcDef));
            classDeclNode.varDeclCount = 0;
            classDeclNode.funcDefCount = 0;
            if (classDecl(&classDeclNode) == 1) {
                // to-do
                classDeclNode.init = true;
                progNode->classDecl[++progNode->classDeclCount] = classDeclNode;
            } else {
                printParserLog("Error occured in class declaration");
            }
        } else {
            addErrorLog("Program should start with class defintion!", currLineNo);
        }
        getNextToken();
    }
    progNode->init = true;
}

void parseStart() {
    initTokenFile();
    generateString("global _start\n\n",scope);
    generateLabelCode("_start",scope);

    ProgNode progNode;
    progNode.init = false;
    parse(&progNode);
    printAllSymbolTables();
    printErrorLogs();
    printScopeElements();
    printPhase("CODE GENERATION");
    parseAST(&progNode);
    
    // test();
}