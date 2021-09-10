#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 

typedef struct _StatementNode StatementNode;
typedef struct _VariableNode VariableNode;

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


void parseVarAssignStmt(VarAssignStmtNode *varAssignStmtNode) {
    if (varAssignStmtNode->init = true) {
        printf("\tMOV EAX, [%d]\n", getVarLocation(varAssignStmtNode->variable.id));
        printf("\tMOV EBX, [%d]\n", getVarLocation(varAssignStmtNode->expr.arithExpr.term.factor.variable->id));
        printf("\tMOV EAX, EBX\n");
    }
}

void parseFuncDef(FuncDefNode *funcDefNode) {
    FuncBodyNode funcBodyNode = funcDefNode->funcBody;
    // statements
    if (funcBodyNode.statementCount > 0){
        if (funcBodyNode.init == true) {
            for (int i = 1; i < funcBodyNode.statementCount + 1; i++) {
                switch (funcBodyNode.statement[i]->statementType) {
                    case 0:
                        // variable assignment
                        parseVarAssignStmt(&funcBodyNode.statement[i]->varAssignStmt);
                        break;
                    
                    default:
                        break;
                }
            }
        }
    }
}

void parseVarDecl(VarDeclNode *varDeclNode) {
    // nothing for now
}


void parseAST(ProgNode *progNode) {
    printf("global _start\n\n");
    printf("_start:\n");
    if (progNode->classDeclCount > 0) {
        // class declarations
        for (int i = 1; i < progNode->classDeclCount + 1; i++) {
            if (progNode->classDecl[i].init == true)
            {
                printf("\tCALL %s\n", progNode->classDecl[i].id);
            }
        }
        for (int i = 1; i < progNode->classDeclCount + 1; i++) {
            if (progNode->classDecl[i].init == true) {
                printf("%s:\n", progNode->classDecl[i].id);
                // variable declaraions
                if (progNode->classDecl[i].varDeclCount > 0) {
                    parseVarDecl(progNode->classDecl[i].varDecl);
                }
                // function definitions
                if (progNode->classDecl[i].funcDefCount > 0) {
                    for (int i = 0; i < progNode->classDecl[i].funcDefCount + 1; i++)
                    {
                        parseFuncDef(&progNode->classDecl[i].funcDef[i]);
                    }
                }    
            }
        }
    }
    printf("\n\n");
    printf("a-%s\n", progNode->classDecl[1].funcDef[1].funcBody.statement[1]->varAssignStmt.variable.id);
    printf("b-%s", progNode->classDecl[1].funcDef[1].funcBody.statement[1]->varAssignStmt.expr.arithExpr.term.factor.variable->id);
}























// void testTree(ProgNode progNode) {
//     if (progNode.init = true)
//     {
//         for (int i = 0; i < progNode.classDeclCount; i++)
//         {
//             printf("CLASS %d : %s\n", i, progNode.classDecl[i].id);
//             for (int j = 0; j < progNode.classDecl[i].varDeclCount; j++)
//             {
//                 printf("VAR INIT: %d\n", progNode.classDecl[i].varDecl->init);
//                 if(progNode.classDecl[i].varDecl->init == true) {
//                     printf("VARIABLE - %d IS %d : %s\n", i, j, progNode.classDecl[i].varDecl[j].id);
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
//     // printf("NODE ONE NEW: %s\n", nt.one.a);
// }

void test() {
    // nodeOne none;
    // none.init = true;
    // strcpy(none.a, "before");
    // nodeTwo ntwo;
    // ntwo.one = none;
    // printf("NONE BEFORE: %s\n", ntwo.one.a);
    // next(&ntwo);
    // printf("NONE AFTER: %s\n", ntwo.one.a);

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
    // printf("DONE");
}