#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _StatementNode StatementNode;
typedef struct _VariableNode VariableNode;
int loopCount = 0;
FILE *assemblyFilePtr;

typedef struct{
    bool init;
    int sizeCount;
    char size[10];
} ArraySizeNode;

typedef struct{
    bool init;
    int typeCount;
    int idCount;
    int arraySizeCount;
    char type[10];
    char id[10];
    ArraySizeNode arraySize[2];
} VarDeclNode;

typedef struct{
    bool init;
    VariableNode *variable;
    char num[10];
} FactorNode;

typedef struct{
    bool init;
    FactorNode factor;
} TermNode;

typedef struct{
    bool init;
    TermNode term;
} ArithExprNode;

typedef struct{
    bool init;
    int arithExprCount;
    ArithExprNode arithExpr[10];
} IndiceNode;

typedef struct{
    bool init;
    int idCount;
    int indiceCount;
    char id[10];
    IndiceNode indiceNode[10];
} IdnestNode;

typedef struct _VariableNode{
    bool init;
    int idnestCount;
    int indiceCount;
    IdnestNode idnestNode[10];
    char id[10];
    IndiceNode indiceNode[10];
} VariableNode;

typedef struct{
    bool init;
    ArithExprNode arithExpr1;
    char relOp[4];
    ArithExprNode arithExpr2;
} RelArithExprNode;

typedef struct{
    bool init;
    ArithExprNode arithExpr;
    RelArithExprNode RelArithExprNode;
} ExprNode;

typedef struct{
    bool init;
    VariableNode variable;
    ExprNode expr;
} VarAssignStmtNode;

typedef struct{
    bool init;
    int statementNodeCount;
    StatementNode *statement[10];
} StatBlockNode;

typedef struct{
    bool init;
    ExprNode expr;
    StatBlockNode statBlock1;
    StatBlockNode statBlock2;
} IfElseStmtNode;

typedef struct{
    bool init;
    ExprNode expr;
    StatBlockNode statBlock;
} WhileStmtNode;

typedef struct{
    bool init;
    VariableNode variable;
} ReadStmtNode;

typedef struct{
    bool init;
    ExprNode expr;
} WriteStmtNode;

typedef struct{
    bool init;
    ExprNode expr;
} ReturnStmtNode;

typedef struct _StatementNode{
    bool init;
    int statementType;
    VarAssignStmtNode varAssignStmt;    // type 0
    IfElseStmtNode ifElseStmtNode;      // type 1
    WhileStmtNode whileStmtNode;        // type 2
    ReadStmtNode readStmtNode;          // type 3
    WriteStmtNode writeStmtNode;        // type 4
    ReturnStmtNode returnStmtNode;      // type 5
} StatementNode;

typedef struct{
    bool init;
    int varCount;
    int statementCount;
    VarDeclNode varDecl[10];
    StatementNode* statement[10];
} FuncBodyNode;

typedef struct{
    bool init;
    char type[10];
    char id[10];
} FuncHeadNode;

typedef struct{
    bool init;
    FuncHeadNode funcHead;
    FuncBodyNode funcBody;
} FuncDefNode;

typedef struct{
    bool init;
} ProgBodyNode;

typedef struct{
    bool init;
    char id[10];
    int varDeclCount;
    int funcDefCount;
    VarDeclNode varDecl[10];
    FuncDefNode funcDef[10];
} ClassDeclNode;

typedef struct{
    bool init;
    int classDeclCount;
    ClassDeclNode classDecl[10];
    ProgBodyNode progBody;
} ProgNode;

void parseReadStmt(ReadStmtNode *readStmtNode) {
    fprintf(assemblyFilePtr, "\t;READ STMT\n");
    fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(readStmtNode->variable.id));
}

void parseWriteStmt(WriteStmtNode *writeStmtNode) {
    fprintf(assemblyFilePtr, "\t;WRITE STMT\n");
    fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(writeStmtNode->expr.arithExpr.term.factor.variable->id));
}

void parseReturnStmt(ReturnStmtNode *returnStmtNode) {
    fprintf(assemblyFilePtr, "\t;RETURN STMT\n");
    if(returnStmtNode->expr.arithExpr.init == true) {
        fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(returnStmtNode->expr.arithExpr.term.factor.variable->id));
    } else {
        fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(returnStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.variable->id));
        fprintf(assemblyFilePtr, "\tMOV EBX, [%d]\n", getVarLocation(returnStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.variable->id));
        fprintf(assemblyFilePtr, "\tCMP EAX, EBX\n");
    }
}

void parseVarAssignStmt(VarAssignStmtNode *varAssignStmtNode) {
    if (varAssignStmtNode->init == true) {        
        fprintf(assemblyFilePtr, "\t;VAR ASSIGNMENT STMT NODE\n");
        fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(varAssignStmtNode->variable.id));
        if (strlen(varAssignStmtNode->expr.arithExpr.term.factor.num) != 0) {
            fprintf(assemblyFilePtr, "\tMOV EBX, %s\n", varAssignStmtNode->expr.arithExpr.term.factor.num);
        } else {
            fprintf(assemblyFilePtr, "\tMOV EBX, [%d]\n", getVarLocation(varAssignStmtNode->expr.arithExpr.term.factor.variable->id));
        }
        
        fprintf(assemblyFilePtr, "\tMOV EAX, EBX\n");
    }
}

void parseWhileStmt(WhileStmtNode *whileStmtNode) {
    fprintf(assemblyFilePtr, "L%d:\n", ++loopCount);

    if (whileStmtNode->init == true) {
        fprintf(assemblyFilePtr, "\t;WHILE STMT NODE\n");
    }

    if (strlen(whileStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.num) != 0) {
            fprintf(assemblyFilePtr, "\tMOV EAX, %s\n", whileStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.num);
    } else {
        fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(whileStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.variable->id));
    }

    if (strlen(whileStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.num) != 0) {
            fprintf(assemblyFilePtr, "\tMOV EBX, %s\n", whileStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.num);
    } else {
        fprintf(assemblyFilePtr, "\tMOV EBX, [%d]\n", getVarLocation(whileStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.variable->id));
    }

    fprintf(assemblyFilePtr, "\tJNE L%d\n\n", ++loopCount);

    for (int i = 1; i < whileStmtNode->statBlock.statementNodeCount + 1; i++) {
        if (whileStmtNode->statBlock.statement[i]->varAssignStmt.init == true) {
            parseVarAssignStmt(&whileStmtNode->statBlock.statement[i]->varAssignStmt);
            whileStmtNode->statBlock.statement[i]->varAssignStmt.init = false;
        } else if (whileStmtNode->statBlock.statement[i]->ifElseStmtNode.init == true) {
            whileStmtNode->statBlock.statement[i]->ifElseStmtNode.init = false;
        } else if (whileStmtNode->statBlock.statement[i]->whileStmtNode.init == true) {
            whileStmtNode->statBlock.statement[i]->whileStmtNode.init = false;
        } else if (whileStmtNode->statBlock.statement[i]->readStmtNode.init == true) {
            parseReadStmt(&whileStmtNode->statBlock.statement[i]->readStmtNode);
            whileStmtNode->statBlock.statement[i]->readStmtNode.init = false;
        } else if (whileStmtNode->statBlock.statement[i]->writeStmtNode.init == true) {
            parseWriteStmt(&whileStmtNode->statBlock.statement[i]->writeStmtNode);
            whileStmtNode->statBlock.statement[i]->writeStmtNode.init = false;
        } else if (whileStmtNode->statBlock.statement[i]->returnStmtNode.init == true) {
            parseReturnStmt(&whileStmtNode->statBlock.statement[i]->returnStmtNode);
            whileStmtNode->statBlock.statement[i]->returnStmtNode.init = false;
        }
    }

    fprintf(assemblyFilePtr, "\tJMP L%d\n\n", loopCount - 1);

    fprintf(assemblyFilePtr, "L%d:\n", loopCount);
}

void parseIfElseStmt(IfElseStmtNode *ifElseStmtNode) {
    if (ifElseStmtNode->init == true) {
        fprintf(assemblyFilePtr, "\t;IF ELSE STMT NODE\n");
        if (strlen(ifElseStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.num) != 0) {
            fprintf(assemblyFilePtr, "\tMOV EAX, %s\n", ifElseStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.num);
        } else {
            fprintf(assemblyFilePtr, "\tMOV EAX, [%d]\n", getVarLocation(ifElseStmtNode->expr.RelArithExprNode.arithExpr1.term.factor.variable->id));
        }

        if (strlen(ifElseStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.num) != 0) {
            fprintf(assemblyFilePtr, "\tMOV EBX, %s\n", ifElseStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.num);
        } else {
            fprintf(assemblyFilePtr, "\tMOV EBX, [%d]\n", getVarLocation(ifElseStmtNode->expr.RelArithExprNode.arithExpr2.term.factor.variable->id));
        }
        
        fprintf(assemblyFilePtr, "\tCMP EAX, EBX\n");
        fprintf(assemblyFilePtr, "\tJE L%d\n", ++loopCount);

        // statblock for if
        for (int i = 1; i < ifElseStmtNode->statBlock1.statementNodeCount + 1; i++) {
            if (ifElseStmtNode->statBlock1.statement[i]->varAssignStmt.init == true) {
                parseVarAssignStmt(&ifElseStmtNode->statBlock1.statement[i]->varAssignStmt);
                ifElseStmtNode->statBlock1.statement[i]->varAssignStmt.init = false;
            } else if (ifElseStmtNode->statBlock1.statement[i]->ifElseStmtNode.init == true) {
                ifElseStmtNode->statBlock1.statement[i]->ifElseStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock1.statement[i]->whileStmtNode.init == true) {
                ifElseStmtNode->statBlock1.statement[i]->whileStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock1.statement[i]->readStmtNode.init == true) {
                parseReadStmt(&ifElseStmtNode->statBlock1.statement[i]->readStmtNode);
                ifElseStmtNode->statBlock1.statement[i]->readStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock1.statement[i]->writeStmtNode.init == true) {
                parseWriteStmt(&ifElseStmtNode->statBlock1.statement[i]->writeStmtNode);
                ifElseStmtNode->statBlock1.statement[i]->writeStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock1.statement[i]->returnStmtNode.init == true) {
                parseReturnStmt(&ifElseStmtNode->statBlock1.statement[i]->returnStmtNode);
                ifElseStmtNode->statBlock1.statement[i]->returnStmtNode.init = false;
            }
        }

        fprintf(assemblyFilePtr, "\nL%d:\n", loopCount++);

        for (int i = 1; i < ifElseStmtNode->statBlock2.statementNodeCount + 1; i++) {
            if (ifElseStmtNode->statBlock2.statement[i]->varAssignStmt.init == true) {
                parseVarAssignStmt(&ifElseStmtNode->statBlock2.statement[i]->varAssignStmt);
                ifElseStmtNode->statBlock2.statement[i]->varAssignStmt.init = false;
            } else if (ifElseStmtNode->statBlock2.statement[i]->ifElseStmtNode.init == true) {
                ifElseStmtNode->statBlock2.statement[i]->ifElseStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock2.statement[i]->whileStmtNode.init == true) {
                ifElseStmtNode->statBlock2.statement[i]->whileStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock2.statement[i]->readStmtNode.init == true) {
                parseReadStmt(&ifElseStmtNode->statBlock2.statement[i]->readStmtNode);
                ifElseStmtNode->statBlock1.statement[i]->readStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock2.statement[i]->writeStmtNode.init == true) {
                parseWriteStmt(&ifElseStmtNode->statBlock2.statement[i]->writeStmtNode);
                ifElseStmtNode->statBlock2.statement[i]->writeStmtNode.init = false;
            } else if (ifElseStmtNode->statBlock2.statement[i]->returnStmtNode.init == true) {
                parseReturnStmt(&ifElseStmtNode->statBlock2.statement[i]->returnStmtNode);
                ifElseStmtNode->statBlock2.statement[i]->returnStmtNode.init = false;
            }
        }

        fprintf(assemblyFilePtr, "\tRET\n");
    }
}

void parseFuncDef(FuncDefNode *funcDefNode) {
    FuncBodyNode funcBodyNode = funcDefNode->funcBody;
    // statements
    if (funcBodyNode.statementCount > 0){
        if (funcBodyNode.init == true) {
            for (int i = 1; i < funcBodyNode.statementCount + 1; i++) {
                if (funcBodyNode.statement[i]->varAssignStmt.init == true) {
                    parseVarAssignStmt(&funcBodyNode.statement[i]->varAssignStmt);
                    funcBodyNode.statement[i]->varAssignStmt.init = false;
                } else if (funcBodyNode.statement[i]->ifElseStmtNode.init == true) {
                    parseIfElseStmt(&funcBodyNode.statement[i]->ifElseStmtNode);
                    funcBodyNode.statement[i]->ifElseStmtNode.init = false;
                } else if (funcBodyNode.statement[i]->whileStmtNode.init == true) {
                    parseWhileStmt(&funcBodyNode.statement[i]->whileStmtNode);
                    funcBodyNode.statement[i]->whileStmtNode.init = false;
                } else if (funcBodyNode.statement[i]->readStmtNode.init == true) {
                    parseReadStmt(&funcBodyNode.statement[i]->readStmtNode);
                    funcBodyNode.statement[i]->whileStmtNode.init == false;
                } else if (funcBodyNode.statement[i]->returnStmtNode.init == true) {
                    parseReturnStmt(&funcBodyNode.statement[i]->returnStmtNode);
                    funcBodyNode.statement[i]->returnStmtNode.init == false;
                } else if (funcBodyNode.statement[i]->writeStmtNode.init == true) {
                    parseWriteStmt(&funcBodyNode.statement[i]->writeStmtNode);
                    funcBodyNode.statement[i]->writeStmtNode.init == false;
                }
            }
        }
    }
}

void parseVarDecl(VarDeclNode *varDeclNode) {
    // nothing for now
}

FILE *getAssemblyFile() {
    assemblyFilePtr = fopen("assembly.txt", "a");
    if (assemblyFilePtr == NULL) {
        fprintf(assemblyFilePtr, "[ERROR] Assembly File Not Found!");
        exit(1);
    }
}

void parseAST(ProgNode *progNode) {
    getAssemblyFile();
    fprintf(assemblyFilePtr, "global _start\n\n");
    fprintf(assemblyFilePtr, "_start:\n");
    if (progNode->classDeclCount > 0) {
        // class declarations
        for (int i = 1; i < progNode->classDeclCount + 1; i++) {
            if (progNode->classDecl[i].init == true)
            {
                fprintf(assemblyFilePtr, "\tCALL %s\n", progNode->classDecl[i].id);
            }
        }
        fprintf(assemblyFilePtr, "\n");
        for (int i = 1; i < progNode->classDeclCount + 1; i++) {
            if (progNode->classDecl[i].init == true) {
                fprintf(assemblyFilePtr, "%s:\n", progNode->classDecl[i].id);
                // variable declaraions
                if (progNode->classDecl[i].varDeclCount > 0) {
                    parseVarDecl(progNode->classDecl[i].varDecl);
                }
                // function definitions
                if (progNode->classDecl[i].funcDefCount > 0) {
                    for (int j = 0; j < progNode->classDecl[i].funcDefCount + 1; j++) {
                        parseFuncDef(&progNode->classDecl[i].funcDef[j]);
                    }
                }    
            }
        }
    }
    fprintf(assemblyFilePtr, "\n\n");
}























// void testTree(ProgNode progNode) {
//     if (progNode.init = true)
//     {
//         for (int i = 0; i < progNode.classDeclCount; i++)
//         {
//             fprintf(assemblyFilePtr, "CLASS %d : %s\n", i, progNode.classDecl[i].id);
//             for (int j = 0; j < progNode.classDecl[i].varDeclCount; j++)
//             {
//                 fprintf(assemblyFilePtr, "VAR INIT: %d\n", progNode.classDecl[i].varDecl->init);
//                 if(progNode.classDecl[i].varDecl->init == true) {
//                     fprintf(assemblyFilePtr, "VARIABLE - %d IS %d : %s\n", i, j, progNode.classDecl[i].varDecl[j].id);
//                 }
//             }
            
//         }
        
//     }
    
// }



// typedef struct{
//     bool init;
//     char a[10];
// } nodeOne;

// typedef struct{
//     nodeOne one;
// } nodeTwo;

// void next(nodeTwo *nt) {
//     strcpy(nt->one.a, "after");
//     // fprintf(assemblyFilePtr, "NODE ONE NEW: %s\n", nt.one.a);
// }

void test() {
    // nodeOne none;
    // none.init = true;
    // strcpy(none.a, "before");
    // nodeTwo ntwo;
    // ntwo.one = none;
    // fprintf(assemblyFilePtr, "NONE BEFORE: %s\n", ntwo.one.a);
    // next(&ntwo);
    // fprintf(assemblyFilePtr, "NONE AFTER: %s\n", ntwo.one.a);

    // ClassDeclNode cdn;
    // ProgBodyNode pbn;
    // FuncDefNode fdn;
    // FuncHeadNode fhn;
    // FuncBodyNode fbn;
    // StatementNode sn;
    // ReturnStmtNode rstn;
    // WriteStmtNode wsn;
    // ReadStmtNode rsn;
    // WhileStmtNode whsn;
    // IfElseStmtNode iesn;
    // StatBlockNode sbln;
    // VarAssignStmtNode vasn;
    // ExprNode en;
    // RelArithExprNode raen;
    // VariableNode vn;
    // IdnestNode idnst;
    // IndiceNode indn;
    // ArithExprNode aen;
    // TermNode tn;
    // FactorNode fn;
    // VarDeclNode vdcln;
    // ArraySizeNode aszn;
    // fprintf(assemblyFilePtr, "DONE");
}