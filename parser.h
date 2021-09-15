#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "symbolTable.h"
#include "AST.h"

FILE *tokensFilePtr;
char line[255];
char* token;
char* currToken;
char* currTokenType;
char* currLineNo;
char* prevToken;
char* prevTokenType;
char* prevLineNo;

ProgNode progNode;

ArithExprNode arithExprNodes[100];
RelArithExprNode relArithExprNodes[100];
TermNode termNodes[100];
FactorNode factorNodes[100];
VariableNode variableNodes[100];
StatBlockNode statBlockNodes[100];
StatementNode statementNodes[100];
ExprNode exprNodes[100];
ReturnStmtNode returnStatementNodes[100];
ClassDeclNode classDeclNodes[100];
VarDeclNode varDeclNodes[100];
FuncDefNode funcDefNodes[100];
FuncHeadNode funcHeadNodes[100];
FuncBodyNode funcBodyNodes[100];
IfElseStmtNode ifElseStmtNodes[100];
VarAssignStmtNode varAssignStmtNodes[100];

int arithExprNodesCount = 0;
int relArithExprNodesCount = 0;
int termNodesCount = 0;
int factorNodesCount = 0;
int variableNodesCount = 0;
int statBlockNodesCount = 0;
int statementNodesCount = 0;
int exprNodesCount = 0;
int returnStatementNodesCount = 0;
int classDeclNodesCount = 0;
int varDeclNodesCount = 0;
int funcDefNodesCount = 0;
int funcHeadNodesCount = 0;
int funcBodyNodesCount = 0;
int ifElseStmtNodesCount = 0;
int varAssignStmtNodesCount = 0;

void initTokenFile() {
    tokensFilePtr = fopen("tokens.txt", "r");
    if (tokensFilePtr == NULL) {
        printf("[ERROR] Token Output File Not Found!");
        exit(1);
    }
}

char *getNextToken() {
    token = fgets(line, 255, tokensFilePtr);
    prevLineNo = currLineNo;
    prevToken = currToken;
    prevTokenType = currTokenType;
    currTokenType = strtok(token, ":");  
    currToken = strtok(NULL, ":");
    currLineNo = strtok(NULL, ":"); 
}

int type() {
    if (strcmp(currToken, "integer") == 0 || 
        strcmp(currToken, "real") == 0    || 
        strcmp(currToken, "id") == 0) {
        addParserLog("Valid type detected...");
        return 1;
    } else {
        addParserLog("Invalid type detected");
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
                if (strcmp(currToken, ";") == 0) {
                    // insert into variable symbol table
                    addParserLog("Variable declaration successful!");
                    strcpy(variable.value, "null");
                    if(insertVariableSymbolTableValue(variable, currLineNo) == 0) {
                        addParserLog("Update variable symbol table failed...");
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
        addParserLog("Variable declaration successful!");
        strcpy(variable.size, "10");
        strcpy(variable.value, "null");
        if(insertVariableSymbolTableValue(variable, currLineNo) == 0) {
            addParserLog("Update variable symbol table failed...");
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
    addParserLog("Checking for an indice");
    if(strcmp(currToken, "[") == 0) {
        getNextToken();
        addParserLog("Checking for an arithmatic expression");
        ArithExprNode arithExprNode0;
        arithExprNode0.init = false;
        if (arithExpr(&arithExprNode0) == 1) {
            arithExprNode0.init = false;
            addParserLog("Arithmatic expression detected");
            getNextToken();
            if (strcmp(currToken, "]") == 0) {
                addParserLog("Indice declared succesfully");
                return 1;
            } else {
                addParserLog("Invalid indice definition, expected a ] after arithmatic expression");
                addErrorLog("Invalid indice definition, expected a ] after arithmatic expression", currLineNo);
                return 0;
            }   
        } else {
            addParserLog("Invalid indice definition, expected an arithmatic expression after [");
            addErrorLog("Invalid indice definition, expected an arithmatic expression after [", currLineNo);
            return 0;
        }
    } else {
        addParserLog("Invalid indice definition, should start with [");
        return 0;
    }
}

int idnest() {
    addParserLog("Checking for an idnest");
    if(strcmp(currTokenType, "ID") == 0) {
        addParserLog("Valid ID detected inside idnest, checking for indice");
        getNextToken();
        if(indice() == 1) {
            addParserLog("Valid indice detected, checking for more indice definition if available");
            getNextToken();
            while (indice() == 1) {
                addParserLog("Looping indices inside idnest");
                getNextToken();
            }
            if(strcmp(currToken, ".") == 0) {
                addParserLog("Valid idnest definition found");
                return 1;
            }
        }
    } else {
        addParserLog("Invalid idnest definition, should start with an ID");
        return 0;
    }
}

int variable(VariableNode *variableNode) {
    addParserLog("Checking for a variable");
    // at this point ID and next token would have been fetched
    if(strcmp(currTokenType, "ID") == 0) {
        strcpy(variableNode->id,currToken);
        variableNode->init = true;
        return 1;
    } else {
        addParserLog("No variable definitions found");
        return 0;
    }
}

int sign() {
    addParserLog("Checking for sign");
    if (strcmp(currToken, "+") == 0 ||
        strcmp(currToken, "-") == 0) {
        return 1;
    } else {
        addParserLog("Sign check failed");
        return 0;
    }
}

int factor(FactorNode *factorNode) {
    variableNodes[++variableNodesCount].init = false;
    memset(variableNodes[variableNodesCount].idnestNode, 0, sizeof(variableNodes[variableNodesCount].idnestNode));
    memset(variableNodes[variableNodesCount].indiceNode, 0, sizeof(variableNodes[variableNodesCount].indiceNode));
    variableNodes[variableNodesCount].idnestCount = 0;
    variableNodes[variableNodesCount].indiceCount = 0;

    IdnestNode idnestNode;
    idnestNode.init = false;
    memset(idnestNode.indiceNode, 0, sizeof(idnestNode.indiceNode));
    idnestNode.indiceCount = 0;
    if (variable(&variableNodes[variableNodesCount]) == 1) {
        factorNode->variable = &variableNodes[variableNodesCount];
        // no need to getNextToken() afer this
        addParserLog("Valid variable definition found, factor defintion success");
        return 1;
    } else if(strcmp(currTokenType, "NUM") == 0 ||
                strcmp(currTokenType, "INTEGER") == 0 ||
                strcmp(currTokenType, "FRACTION") == 0 ||
                strcmp(currTokenType, "DIGIT") == 0 ||
                strcmp(currTokenType, "NONZERO") == 0) {
        addParserLog("A valid ID detected, factor definition success");
        strcpy(factorNode->num, currToken);
        return 1;
    } else if (idnest(&idnestNode) == 1) {
        addParserLog("Valid idnest detected, checking for more idnest definition if available");
        while(idnest() == 1) {
            addParserLog("More idnest definition found");
            getNextToken();
        }
        if(strcmp(currTokenType, "ID") == 0) {
            addParserLog("A valid ID detected, checking (aParamas)");
            getNextToken();
            if(strcmp(currToken, "(") == 0) {
                addParserLog("Checking for aParams");
                getNextToken();
                while (strcmp(currToken, ")") == 0){
                    if(aParams() == 1) {
                        addParserLog("aParams detected, checking for more");
                        getNextToken();
                        while(aParams() == 1) {
                            addParserLog("More aParams detected");
                            getNextToken();
                        }
                    }
                    if(strcmp(currToken, ")") == 0) {
                        addParserLog("Valid factor definition found");
                        return 1;
                    } else {
                        getNextToken();
                    }
                }
                
            } else {
                addParserLog("Invalid Factor definition, an ID should be followed by (aParams)");
                addErrorLog("Invalid Factor definition, an ID should be followed by (aParams)", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid Factor definition, an idnest should be followed by an ID");
            addErrorLog("Invalid Factor definition, an idnest should be followed by an ID", currLineNo);
            return 0;
        }
    } else if(strcmp(currTokenType, "ID") == 0) {
        addParserLog("A valid ID detected, checking (aParamas)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Checking for aParams");
            getNextToken();
            while (strcmp(currToken, ")") == 0){
                if(aParams() == 1) {
                    addParserLog("aParams detected, checking for more");
                    getNextToken();
                    while(aParams() == 1) {
                        addParserLog("More aParams detected");
                        getNextToken();
                    }
                }

                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Valid factor definition found");
                    return 1;
                } else {
                    getNextToken();
                }
            }
            
        } else {
            addParserLog("Invalid Factor definition, an ID should be followed by (aParams)");
            addErrorLog("Invalid Factor definition, an ID should be followed by (aParams)", currLineNo);
            return 0;
        }

    } else if(strcmp(currToken, "(") == 0) {
        addParserLog("Checking for (expression)");
        getNextToken();
        ExprNode exprNode;
        exprNode.init = false;
        if (expr(&exprNode) == 1) {
            exprNode.init = true;
            addParserLog("Expression detected");
            if(strcmp(currToken, ")") == 0) {
                addParserLog("A valid (expression) detected, factor definition success");
            } else {
                addParserLog("Invalid Factor definition, an expression should be ecnlosed within ()");
                addErrorLog("Invalid Factor definition, an expression should be ecnlosed within ()", currLineNo);
                return 0;
            }
        }
        
    } else if (strcmp(currTokenType, "NOT") == 0) {
        addParserLog("Detected NOT, checking for factor");
        getNextToken();
        FactorNode factorNode;
        factorNode.init = true;
        if (factor(&factorNode) == 1) {
            factorNode.init = false;
            addParserLog("A valid factor definition found");
            return 1;
        } else {
            addParserLog("Invalid Factor definition, a NOT should be followed by a factor");
            addErrorLog("Invalid Factor definition, a NOT should be followed by a factor", currLineNo);
            return 0;
        }
    } else if (sign() == 1) {
        addParserLog("Detected sign, checking for factor");
        getNextToken();
        FactorNode factorNode;
        factorNode.init = true;
        if (factor(&factorNode) == 1) {
            factorNode.init = false;
            addParserLog("A valid factor definition found");
            return 1;
        } else {
            addParserLog("Invalid Factor definition, a sign should be followed by a factor");
            addErrorLog("Invalid Factor definition, a sign should be followed by a factor", currLineNo);
            return 0;
        }
        
    }
    return 1;
}

int term(TermNode *termNode) {
    factorNodes[++factorNodesCount].init = false;
    if(factor(&factorNodes[factorNodesCount]) == 1) {
        factorNodes[factorNodesCount].init = true;
        termNode->factor = factorNodes[factorNodesCount];
        addParserLog("Detected factor, checking for mult operator");
        getNextToken();
        if(multOp() == 1) {
            addParserLog("Multiplication operation detected");
            getNextToken();
            addParserLog("Checking for a factor");
            if(factor(&factorNodes[++factorNodesCount]) == 1) {
                addParserLog("Factor detected");
                return 1;
            } else {
                addParserLog("A multiplication operator should be followed by a factor for a term");
                addErrorLog("Invalid term declaration, a multiplication operator should be followed by a factor for a term", currLineNo);
                return 0;
            }
        } else {
            return 1;
        }
    } else {
        addParserLog("Invalid term declaration");
        addErrorLog("Invalid term declaration found, expected a term or a factor", currLineNo);
        return 0;
    }
}

int arithExpr(ArithExprNode *arithExprNode) {
    addParserLog("Checking arithmatic expression");
    termNodes[++termNodesCount].init = false;
    if(term(&termNodes[termNodesCount]) == 1) {
        termNodes[termNodesCount].init = true;
        arithExprNode->term = termNodes[termNodesCount];
        addParserLog("Detected term");
        return 1;
    } else if(addOp() == 1) {
        addParserLog("Add operation detected inside arithmatic expression");
        getNextToken();
        if(term(&termNodes[++termNodesCount]) == 1) {
            termNodes[termNodesCount].init = true;
            addParserLog("Detected term after add operation");
            return 1;
        } else {
            addParserLog("Invalid arithmatic expression detected, expected a term after add operation");
            addErrorLog("Invalid arithmatic expression detected, expected a term after add operation", currLineNo);
            return 0;
        }
    } else {
        addParserLog("No arithmatic definitions found");
        return 0;
    }
}

int expr(ExprNode *exprNode) {
    addParserLog("Checking for expression defintion");
    arithExprNodes[++arithExprNodesCount].init = false;
    if (arithExpr(&arithExprNodes[arithExprNodesCount]) == 1) {
        arithExprNodes[arithExprNodesCount].init = true;
        exprNode->arithExpr = arithExprNodes[arithExprNodesCount];
        relArithExprNodes[++relArithExprNodesCount].init = false;
        if (relOp() == 1) {
            exprNode->arithExpr.init = false;
            relArithExprNodes[relArithExprNodesCount].arithExpr1 = arithExprNodes[arithExprNodesCount];
            strcpy(relArithExprNodes[relArithExprNodesCount].relOp, currToken);
            addParserLog("Relational operator detected, checking for arithmatic definition");
            getNextToken();
            arithExprNodes[++arithExprNodesCount].init = false;
            if(arithExpr(&arithExprNodes[arithExprNodesCount]) == 1) {
                arithExprNodes[arithExprNodesCount].init = true;
                relArithExprNodes[relArithExprNodesCount].arithExpr2 = arithExprNodes[arithExprNodesCount];
                relArithExprNodes[relArithExprNodesCount].init = true;
                exprNode->RelArithExprNode = relArithExprNodes[relArithExprNodesCount];
                addParserLog("Expression defined successfully");
                return 1;
            } else {
                addParserLog("Invalid expression definition, a relational operator should be followed by an arithmatic operator");
                addErrorLog("Invalid expression definition, a relational operator should be followed by an arithmatic operator", currLineNo);
                return 0;
            }
        } else {
            // no need to call getNextToken() after this method
            addParserLog("Expression defintion successful");
            return 1;
        }
    } else {
        addParserLog("Invalid expression definition, should start with an arithmatic expression");
        addErrorLog("Invalid expression definition, should start with an arithmatic expression", currLineNo);
        return 0;
    }
    
    return 1;
}

int statBlock(StatBlockNode *statBlockNode) {
    addParserLog("Checking for statBlock defintion");
    // StatementNode statementNode;
    statementNodes[++statementNodesCount].init = false;
    if(strcmp(currToken, "{") == 0) {
        addParserLog("Detected { , checking for statement");
        getNextToken();
        if(statement(&statementNodes[statementNodesCount]) == 1) {
            statementNodes[statementNodesCount].init = true;
            statBlockNode->statement[++statBlockNode->statementNodeCount] = &statementNodes[statementNodesCount];
            addParserLog("Detected statement , checking for more statements");
            getNextToken();
            while (statement(&statementNodes[++statementNodesCount]) == 1){
                statBlockNode->statement[++statBlockNode->statementNodeCount] = &statementNodes[statementNodesCount];
                addParserLog("Detected more statements");
                getNextToken();
            }
        } else {
            addParserLog("No statements found inside statBlock");
        }
        if(strcmp(currToken, "}") == 0) {
            addParserLog("statBlock detected succesfully");
            return 1;
        } else {
            addParserLog("Invalid statBlock definition, expected a } at the end");
            addErrorLog("Invalid statBlock definition, expected a } at the end", currLineNo);
            return 0;
        }
    } else if(statement(&statementNodes[statementNodesCount]) == 1){
        statBlockNode->statement[statementNodesCount] = &statementNodes[statementNodesCount];
        addParserLog("statBlock detected succesfully");
        return 1;
    } else {
        addParserLog("Empty statBlock detected without {}");
        return 1;
    }
}

int statement(StatementNode *statementNode) {
    addParserLog("Checking for statement defintion");

    // VariableNode variableNode0;
    variableNodes[++variableNodesCount].init = false;

    if(variable(&variableNodes[variableNodesCount]) == 1) {
        statementNode->statementType = 0;
        variableNodes[variableNodesCount].init = true;
        varAssignStmtNodes[++varAssignStmtNodesCount].init = false;
        varAssignStmtNodes[varAssignStmtNodesCount].variable = variableNodes[variableNodesCount];

        addParserLog("Variable detected inside statement, checking for assignment operator");
        getNextToken();
        if(assignOp() == 1) {
            addParserLog("Assign operator detected, checking for expression");
            getNextToken();
            exprNodes[++exprNodesCount].init = false;
            bool isNum = false;
                if(strcmp(currTokenType, "NUM") == 0 ||
                strcmp(currTokenType, "INTEGER") == 0 ||
                strcmp(currTokenType, "FRACTION") == 0 ||
                strcmp(currTokenType, "DIGIT") == 0 ||
                strcmp(currTokenType, "NONZERO") == 0) {
                    isNum = true;
                }
            
            if(expr(&exprNodes[exprNodesCount]) == 1) {
                if(isNum == true) {
                    variableSym *v = getVarriableSymbol(varAssignStmtNodes[varAssignStmtNodesCount].variable.id);
                    printf("VAR %s\n", v->name);
                    strcpy(v->value, exprNodes[exprNodesCount].arithExpr.term.factor.num);
                    updateVariableSymbolTableValue(v);   
                }

                exprNodes[exprNodesCount].init = true;
                varAssignStmtNodes[varAssignStmtNodesCount].expr = exprNodes[exprNodesCount];
                addParserLog("Expression detected, checking for ;");
                // getNextToken();
                // getNextToken();
                if(strcmp(currToken, ";") == 0) {
                    varAssignStmtNodes[varAssignStmtNodesCount].init = true;
                    statementNode->varAssignStmt = varAssignStmtNodes[varAssignStmtNodesCount];
                    statementNode->init = true;
                    addParserLog("Statement definition succesfull");
                    return 1;
                } else {
                    addParserLog("Invalid statement definition, a ; expected after expression");
                    addErrorLog("Invalid statement definition, a ; expected after expression", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid statement definition, a valid expression detected after assignment operator");
                addErrorLog("Invalid statement definition, a valid expression detected after assignment operator", currLineNo);
                return 0;
            }
        }
    } else if (strcmp(currToken, "if") == 0) {
        statementNode->statementType = 1;
        addParserLog("If condition detected, checking for (expression)");
        getNextToken();
        
        // IfElseStmtNode ifElseStmtNode;
        ifElseStmtNodes[++ifElseStmtNodesCount].init = false;
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Detected ( , checking for expression");
            getNextToken();
            // ExprNode exprNode;
            exprNodes[++exprNodesCount].init = false;
            if(expr(&exprNodes[exprNodesCount]) == 1) {
                exprNodes[exprNodesCount].init = true;
                ifElseStmtNodes[ifElseStmtNodesCount].expr = exprNodes[exprNodesCount];
                addParserLog("Detected expression , checking for )");
                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Detected ) , checking for then");
                    getNextToken();
                    if(strcmp(currToken, "then") == 0) {
                        addParserLog("Detected then , checking for statBlock");
                        getNextToken();
                        // StatBlockNode statBlockNode;
                        statBlockNodes[++statBlockNodesCount].init = false;
                        memset(statBlockNodes[statBlockNodesCount].statement, 0, sizeof(statBlockNodes[statBlockNodesCount].statement));
                        statBlockNodes[statBlockNodesCount].statementNodeCount = 0;
                        if(statBlock(&statBlockNodes[statBlockNodesCount]) == 1) {
                            statBlockNodes[statBlockNodesCount].init = true;
                            ifElseStmtNodes[ifElseStmtNodesCount].statBlock1 = statBlockNodes[statBlockNodesCount];
                            addParserLog("Detected statBlock , checking for else");
                            getNextToken();
                            if(strcmp(currToken, "else") == 0) {
                                addParserLog("Detected else , checking for statBlock");
                                getNextToken();
                                statBlockNodes[++statBlockNodesCount].init = false;
                                if(statBlock(&statBlockNodes[statBlockNodesCount]) == 1) {
                                    statBlockNodes[statBlockNodesCount].init = true;
                                    ifElseStmtNodes[ifElseStmtNodesCount].statBlock2 = statBlockNodes[statBlockNodesCount];
                                    addParserLog("Detected statBlock , checking for ;");
                                    getNextToken();
                                    if(strcmp(currToken, ";") == 0) {
                                        ifElseStmtNodes[ifElseStmtNodesCount].init = true;
                                        statementNode->ifElseStmtNode = ifElseStmtNodes[ifElseStmtNodesCount];
                                        statementNode->init = true;
                                        addParserLog("If condition definition succesfull");
                                        return 1;
                                    } else {
                                        addParserLog("Invalid if condition definition, a ; expected at the end");
                                        addErrorLog("Invalid if condition definition, a ; expected at the end", currLineNo);
                                        return 0;
                                    }
                                } else {
                                    addParserLog("Invalid if condition definition, a else should be followed by an statBlock");
                                    addErrorLog("Invalid if condition definition, a else should be followed by an statBlock", currLineNo);
                                    return 0;
                                }
                            } else {
                                addParserLog("Invalid if condition definition, a statBlock should be followed by an else");
                                addErrorLog("Invalid if condition definition, a statBlock should be followed by an else", currLineNo);
                                return 0;
                            }
                        } else {
                            addParserLog("Invalid if condition definition, a statBlock expected after then");
                            addErrorLog("Invalid if condition definition, a statBlock expected after then", currLineNo);
                            return 0;
                        }
                    } else {
                        addParserLog("Invalid if condition definition, an expression should be followed by then");
                        addErrorLog("Invalid if condition definition, an expression should be followed by then", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Invalid if condition definition, an expression should be followed by a )");
                    addErrorLog("Invalid if condition definition, an expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid if condition definition, a ( should be followed by a valid expression");
                addErrorLog("Invalid if condition definition, a ( should be followed by a valid expression", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid if condition definition, if should be followed with (expression)");
            addErrorLog("Invalid if condition definition, if should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "while") == 0) {
        statementNode->statementType = 2;
        WhileStmtNode whileStmtNode;
        whileStmtNode.init = false;
        addParserLog("While condition detected, checking for (expression)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                whileStmtNode.expr = exprNode;
                addParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Detected ) , checking for do");
                    getNextToken();
                    if(strcmp(currToken, "do") == 0) {
                        addParserLog("Detected else , checking for statBlock");
                        getNextToken();
                        StatBlockNode statBlockNode;
                        statBlockNode.init = false;
                        memset(statBlockNode.statement, 0, sizeof(statBlockNode.statement));
                        statBlockNode.statementNodeCount = 0;
                        if(statBlock(&statBlockNode) == 1) {
                            statBlockNode.init = true;
                            whileStmtNode.statBlock = statBlockNode;
                            whileStmtNode.init = true;
                            addParserLog("Detected statBlock , checking for ;");
                            getNextToken();
                            if(strcmp(currToken, ";") == 0) {
                                statementNode->whileStmtNode = whileStmtNode;
                                statementNode->init = true;
                                addParserLog("while condition definition succesfull");
                                return 1;
                            } else {
                                addParserLog("Invalid while condition definition, a ; expected at the end");
                                addErrorLog("Invalid while condition definition, a ; expected at the end", currLineNo);
                                return 0;
                            }
                        } else {
                            addParserLog("Invalid while condition definition, a do should be followed by an statBlock");
                            addErrorLog("Invalid while condition definition, a do should be followed by an statBlock", currLineNo);
                            return 0;
                        }
                    } else {
                        addParserLog("Invalid while condition definition, a statBlock should be followed by a do");
                        addErrorLog("Invalid while condition definition, a statBlock should be followed by a do", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Invalid while condition definition, an expression should be followed by a )");
                    addErrorLog("Invalid while condition definition, an expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid while condition definition, a ( should be followed by a valid expression");
                addErrorLog("Invalid while condition definition, a ( should be followed by a valid expression", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid while condition definition, while should be followed with (expression)");
            addErrorLog("Invalid while condition definition, while should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "read") == 0) {
        statementNode->statementType = 3;
        ReadStmtNode readStmtNode;
        readStmtNode.init = true;
        addParserLog("Read detected, checking for (variable)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Detected ( , checking for variable");
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
                addParserLog("Detected variable , checking for )");
                getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->readStmtNode = readStmtNode;
                        statementNode->init = true;
                        addParserLog("Read definition succesfull");
                        return 1;
                    } else {
                        addParserLog("Invalid read definition, a ; expected at the end");
                        addErrorLog("Invalid read definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Invalid read definition, a variable should be followed by a )");
                    addErrorLog("Invalid read definition, a variable should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid read definition, ( should be followed by a variable");
                addErrorLog("Invalid read definition, ( should be followed by a variable", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid read definition, read should be followed with (variable)");
            addErrorLog("Invalid read definition, read should be followed with (variable)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "write") == 0) {
        statementNode->statementType = 4;
        WriteStmtNode writeStmtNode;
        writeStmtNode.init = false;
        addParserLog("Write detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Detected ( , checking for expression");
            getNextToken();
            ExprNode exprNode;
            exprNode.init = false;
            if(expr(&exprNode) == 1) {
                exprNode.init = true;
                writeStmtNode.expr = exprNode;
                writeStmtNode.init = true;
                addParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->writeStmtNode = writeStmtNode;
                        statementNode->init = true;
                        addParserLog("Write definition succesfull");
                        return 1;
                    } else {
                        addParserLog("Invalid write definition, a ; expected at the end");
                        addErrorLog("Invalid write definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Invalid write definition, a expression should be followed by a )");
                    addErrorLog("Invalid write definition, a expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid write definition, ( should be followed by a expression");
                addErrorLog("Invalid write definition, ( should be followed by a expression", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid write definition, write should be followed with (expression)");
            addErrorLog("Invalid write definition, write should be followed with (expression)", currLineNo);
            return 0;
        }
    } else if (strcmp(currToken, "return") == 0) {
        statementNode->statementType = 5;
        returnStatementNodes[++returnStatementNodesCount].init = false;
        addParserLog("Return detected, checking for (expr)");
        getNextToken();
        if(strcmp(currToken, "(") == 0) {
            addParserLog("Detected ( , checking for expression");
            getNextToken();
            exprNodes[++exprNodesCount].init = false;
            if(expr(&exprNodes[exprNodesCount]) == 1) {
                exprNodes[exprNodesCount].init = true;
                returnStatementNodes[returnStatementNodesCount].expr = exprNodes[exprNodesCount];
                returnStatementNodes[returnStatementNodesCount].init = true;
                addParserLog("Detected expression , checking for )");
                // getNextToken();
                if(strcmp(currToken, ")") == 0) {
                    addParserLog("Detected ) , checking for ;");
                    getNextToken();
                    if(strcmp(currToken, ";") == 0) {
                        statementNode->returnStmtNode = returnStatementNodes[returnStatementNodesCount];
                        statementNode->init = true;
                        addParserLog("Return definition succesfull");
                        return 1;
                    } else {
                        addParserLog("Invalid return definition, a ; expected at the end");
                        addErrorLog("Invalid return definition, a ; expected at the end", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Invalid return definition, a expression should be followed by a )");
                    addErrorLog("Invalid return definition, a expression should be followed by a )", currLineNo);
                    return 0;
                }
            } else {
                addParserLog("Invalid return definition, ( should be followed by a expression");
                addErrorLog("Invalid return definition, ( should be followed by a expression", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Invalid statement definition, expected variable/if/while/read/write/return");
            addErrorLog("Invalid statement definition, expected variable/if/while/read/write/return", currLineNo);
            return 0;
        }
    } else {
        addParserLog("No statements found");
        return 0;
    }
}

int arraySize() {
    if (strcmp(currToken, "[") == 0) {
        addParserLog("A [ detected, checking for Digit");
        getNextToken();
        if (strcmp(currTokenType, "DIGIT") == 0) {
            addParserLog("A Digit detected, checking for ]");
            getNextToken();
            if (strcmp(currToken, "]") == 0) {
                addParserLog("Array definition success");
                return 1;
            } else {
                addParserLog("Invalid array size defintion, ] expected after array size");
                addErrorLog("Invalid array size defintion, ] expected after array size", currLineNo);
                return 0;
            }                        
        } else {
            addParserLog("Invalid array size defintion, expected a digit or int inside []");
            return 0;
        }
    } else {
        addParserLog("Invalid array size defintion, expected a [digit/int]");
        return 0;
    }
}

int aParams() {
    ExprNode exprNode;
    exprNode.init = false;
    if(expr(&exprNode) == 1) {
        exprNode.init = true;
        addParserLog("A valid expression found, checking for more aParams");
        getNextToken();
        while(strcmp(currToken, ",") != 0) {
            if(aParams() == 1) {
                addParserLog("More aParams found");
                getNextToken();
            } else {
                addParserLog("Invalid aParams definition, a comman should be followed by another aParam");
                addErrorLog("Invalid aParams definition, a comman should be followed by another aParam", currLineNo);
                return 0;
            }
        }
    } else {
        addParserLog("Empty aParams found");
        return 1;
    }
}

int fParams() {
    if (type() == 1) {
        addParserLog("A valid type detected, checking for ID");
        getNextToken();
        if (strcmp(currTokenType, "ID") == 0) {
            addParserLog("A valid ID detected, checking for array sizes [int]");
            getNextToken();
            if(arraySize() == 1) {
                addParserLog("A valid array size detected, checking for more array sizes ");
                getNextToken();
                while(strcmp(currToken, ",") == 0) {
                    // fParamsTail
                    getNextToken();
                    if(fParams() == 1) {
                        // continue
                        return 1;
                    } else {
                        addParserLog("No more fParams detected");
                        return 0;
                    }
                }
            } else {
                addParserLog("No array size or fParamsTail detected");
                return 1;
            }
        } else {
            addParserLog("Invalid fParam defintion, type should be followed with an ID...");
            addErrorLog("Invalid fParam defintion, type should be followed with an ID", currLineNo);
            return 0;
        }
    } else {
        addParserLog("Invalid fParam defintion, type is expected...");
        addErrorLog("Invalid type definition, only the types integer, real are allowed", currLineNo);
        addErrorLog("Invalid fParam defintion, type is expected", currLineNo);
        return 0;
    }
}

int funcBody(FuncBodyNode *funcBodyNode) {
    addParserLog("Came inside function body");
    getNextToken();

    if (strcmp(currToken, "{") != 0) {
        addErrorLog("Invalid function body definition, should start with {", currLineNo);    
    }
    getNextToken();
    while (strcmp(currToken, "}") != 0) {
        if (type() == 1) {
            addParserLog("Checking for variable definitions");
            char name[10];
            char type[10];
            strcpy(type, currToken);
            getNextToken();
            if (strcmp(currTokenType, "ID") == 0) {
                addParserLog("Valid ID detected...");
                strcpy(name, currToken);
                getNextToken();
                if (strcmp(currToken, "[") == 0 || strcmp(currToken, "=") == 0 || strcmp(currToken, ";") == 0) {
                    addParserLog("Variable declration detected");
                    varDeclNodes[++varDeclNodesCount].init = false;
                    varDeclNodes[varDeclNodesCount].arraySizeCount = 0;
                    strcpy(varDeclNodes[varDeclNodesCount].id, name);
                    strcpy(varDeclNodes[varDeclNodesCount].type, type);
                    variableSym variable;
                    strcpy(variable.name, name);
                    strcpy(variable.type, type);
                    varDecl(variable, &varDeclNodes[varDeclNodesCount]);
                    varDeclNodes[varDeclNodesCount].init = true;
                    funcBodyNode->varDecl[++funcBodyNode->varCount] = varDeclNodes[varDeclNodesCount];
                }
            } else {
                addParserLog("Invalid variable declaration: a variable name is not specified!");
                addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                return 0;
            }
        } else {
            addParserLog("Type check failed, checking for statement definition");
            // StatementNode statementNode;
            statementNodes[++statementNodesCount].init = false;
            int statCount = statementNodesCount;
            if(statement(&statementNodes[statCount]) == 1) {
                statementNodes[statCount].init = true;
                funcBodyNode->statement[++funcBodyNode->statementCount] = &statementNodes[statCount];
                addParserLog("A valid statement detected, checking for more statements");
                getNextToken();

                statCount = ++statementNodesCount;
                while(statement(&statementNodes[statementNodesCount]) == 1) {
                    statementNodes[statCount].init = true;
                    funcBodyNode->statement[++funcBodyNode->statementCount] = &statementNodes[statCount];
                    addParserLog("More statements detected");
                    getNextToken();
                    statCount = ++statementNodesCount;
                }
            } else {
                addParserLog("Statement detection failed");
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
    addParserLog("Came inside function defintion");

    if(insertFunctionSymbolTableValue(function, currLineNo) == 0) {
        addParserLog("Update function symbol table failed...");
        return 0;
    }

    addParserLog("Function symbol table update succesfully");
    getNextToken();

    // fParams
    while (strcmp(currToken, ")") != 0) {
        addParserLog("Checking for fParams");
        if(fParams() == 1) {

        } else {
            addParserLog("No fParams detected, so checking the funcBody next");
        }
    }

    // FuncBodyNode funcBodyNode;
    funcBodyNodes[++funcBodyNodesCount].init = false;
    funcBodyNodes[funcBodyNodesCount].varCount = 0;
    funcBodyNodes[funcBodyNodesCount].statementCount = 0;
    if(funcBody(&funcBodyNodes[funcBodyNodesCount]) == 1) {
        funcBodyNodes[funcBodyNodesCount].init = true;
        funcDefNode->funcBody = funcBodyNodes[funcBodyNodesCount];
        addParserLog("Function body definition successful, checking for ;");
        getNextToken();
        if(strcmp(currToken, ";") == 0) {
            addParserLog("Function definition successful");
            return 1;
        } else {
            addParserLog("Invalid function definition, expected ; at the end");
            addErrorLog("Invalid function definition, expected ; at the end", currLineNo);
        }
    } else {
        addParserLog("Invalid function body definition");
        addErrorLog("Invalid function body definition", currLineNo);
        return 0;
    }
    return 1;
}

int classDecl(ClassDeclNode *classDeclNode) {
    classSym classSym;
    getNextToken();      
    addParserLog("Came inside class declaration..."); 
    if (strcmp(currTokenType, "ID") == 0) {
        strcpy(classDeclNode->id, currToken);
        strcpy(classSym.name, currToken);
        getNextToken();
        addParserLog("Detected ID..."); 
        if (strcmp(currToken, "{") == 0) {
            getNextToken();
            addParserLog("Came inside class body...");
            insertClassSymbolTableValue(classSym, currLineNo);
            int x=0;
            while (strcmp(currToken, "}") != 0) {
                if (type() == 1) {
                    char name[10];
                    char type[10];
                    strcpy(type, currToken);
                    getNextToken();
                    if (strcmp(currTokenType, "ID") == 0) {
                        addParserLog("Valid ID detected...");
                        strcpy(name, currToken);
                        getNextToken();
                        if (strcmp(currToken, "[") == 0 || strcmp(currToken, "=") == 0 || strcmp(currToken, ";") == 0) {
                            addParserLog("Variable declration detected");
                            varDeclNodes[++variableNodesCount].init = false;
                            memset(varDeclNodes[variableNodesCount].arraySize, 0, sizeof(varDeclNodes[variableNodesCount].arraySize));
                            varDeclNodes[variableNodesCount].arraySizeCount = 0;
                            strcpy(varDeclNodes[variableNodesCount].id, name);
                            strcpy(varDeclNodes[variableNodesCount].type, type);
                            variableSym variable;
                            strcpy(variable.name, name);
                            strcpy(variable.type, type);
                            varDecl(variable, &varDeclNodes[variableNodesCount]);
                            varDeclNodes[variableNodesCount].init = true;
                            classDeclNode->varDecl[++classDeclNode->varDeclCount] = varDeclNodes[variableNodesCount];
                        } else if(strcmp(currToken, "(") == 0) {
                            addParserLog("Function declration detected");
                            funcDefNodes[++funcDefNodesCount].init = false;
                            funcHeadNodes[++funcHeadNodesCount].init = true;
                            strcpy(funcHeadNodes[funcHeadNodesCount].id, name);
                            strcpy(funcHeadNodes[funcHeadNodesCount].type, type);
                            funcDefNodes[funcDefNodesCount].funcHead = funcHeadNodes[funcHeadNodesCount];

                            functionSym function;
                            strcpy(function.name, name);
                            strcpy(function.type, type);

                            funcDef(function, &funcDefNodes[funcDefNodesCount]);
                            funcDefNodes[funcDefNodesCount].init = true;
                            classDeclNode->funcDef[++classDeclNode->funcDefCount] = funcDefNodes[funcDefNodesCount];
                        }
                        getNextToken();
                    } else {
                        addErrorLog("Invalid variable declaration: a variable name is not specified!", currLineNo);
                        return 0;
                    }
                } else {
                    addParserLog("Type check failed...");
                    addErrorLog("Invalid type definition, only the types integer, real are allowed", currLineNo);
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

void parse(ProgNode *progNode) {
    getNextToken();
    while (strcmp(currToken, "EOF") != 0) {
        if (strcmp(currToken, "class") == 0) {
            addParserLog("Found class declaration...");
            classDeclNodes[++classDeclNodesCount].init = false;
            memset(classDeclNodes[classDeclNodesCount].varDecl, 0, sizeof(classDeclNodes[classDeclNodesCount].varDecl));
            memset(classDeclNodes[classDeclNodesCount].funcDef, 0, sizeof(classDeclNodes[classDeclNodesCount].funcDef));
            classDeclNodes[classDeclNodesCount].varDeclCount = 0;
            classDeclNodes[classDeclNodesCount].funcDefCount = 0;
            if (classDecl(&classDeclNodes[classDeclNodesCount]) == 1) {
                // to-do
                classDeclNodes[classDeclNodesCount].init = true;
                progNode->classDecl[++progNode->classDeclCount] = classDeclNodes[classDeclNodesCount];
            } else {
                addParserLog("Error occured in class declaration");
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

    
    progNode.init = false;
    parse(&progNode);
    // printParseLogs();
    printAllSymbolTables();
    // printErrorLogs();
}

void generateCode() {
    parseAST(&progNode);
}