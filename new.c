#include <stdio.h>
#include <string.h>
#include "new.h"
#include "y.tab.h"


typedef enum {
	false, true
} bool;

static int lbl = 0;

int lbl1, lbl2, lbl3, lbl4, lbl5;

static int clk = 12000;

const char * complete(bool b, nodeType *p) {
	char str[16];

	switch (p->type) {
	case typeReg:
		if (b == true) {
			sprintf(str, "%sh", p->regId.ireg);
		} else {
			sprintf(str, "%s", p->regId.ireg);
		}
		break;
	case typePort:
		sprintf(str, "%s", p->portId.iport);
		break;
	case typeCon:
		sprintf(str, "#%d", p->con.value);
		break;
	}
	char *st = str;
	return st;
}

int setclock(nodeType *p) {
	clk = p->con.value;
	return 0;
}

int delay(nodeType *p) {
	printf("\tMOV TMOD, #01h\n");
	float machine_time = 12000 / (float) clk;//Single machine cycle time in us
	float overflow_time = machine_time * 65.535;	//timer max time in ms
	int delay = p->con.value;
	if (delay > overflow_time) {
		int r0 = delay / (int) overflow_time;
		int r1 = 1;
		if (r0 > 255) {
			r1 = r0 / 255;
			r0 = 255;
			if (r1 > 255) {
				yyerror("Index out of range");
				return 0;
			}
		}
		printf("\tSETB RS0\n");
		printf("\tMOV R1,#%d\nTL1:\n", r1);
		printf("\tMOV R0,#%d\nTL2:\n\tCLR TF0\n", r0);
		printf("\tMOV TH0,#0FFh\n\tMOV TL0, #0FFh\n");
		printf("\tSETB TR0\n\tJNB TF0, $\n");
		printf("\tDJNZ R0, TL2\n\tDJNZ R1, TL1\n");
		printf("\tCLR RS0\n");
	} else {
		int tl = delay * 1000 / (int) machine_time;
		int th = 0;
		if (tl > 255) {
			th = tl / 255;
			tl = 255;
		}
		printf("\tCLR TF0\n");
		printf("\tMOV TH0,#%d\n\tMOV TL0, #%d\n", 255 - th, 255 - tl);
		printf("\tSETB TR0\n\tJNB TF0, $\n");
		printf("\tSETB TR0\n\tJNB TF0, $\n");
	}
	return 0;
}

int whilecondition() {
	printf("L%03d:\n", lbl2);
	printf("\tJC L%03d\n", lbl1);
	return 0;
}

int ifcondition() {
	printf("\tSJMP L%03d\n", lbl4 = lbl++);
	printf("L%03d:\n", lbl2);
	printf("\tJC L%03d\n", lbl3);
	printf("L%03d:\n", lbl4);
	return 0;
}

int ifelsecondition() {
	printf("\tSJMP L%03d\n", lbl5);
	printf("L%03d:\n", lbl2);
	printf("\tJC L%03d\n", lbl3);
	printf("\tSJMP L%03d\n", lbl4);
	printf("L%03d:\n", lbl5);
	return 0;
}

int elsecondition() {
	printf("\tSJMP L%03d\n", lbl5 = lbl++);
	printf("L%03d:\n", lbl4 = lbl++);
	return 0;
}

int checkcondition(nodeType *p1, nodeType *logic, nodeType *p2) {
	printf("L%03d:\n\tCLR C\n", lbl1 = lbl++);
	switch (logic->opr.oper) {
	case '<':
		printf("\tMOV A, %s\n", complete(false, p1));
		printf("\tSUBB A, %s\n", complete(false, p2));
		break;
	case '>':
		printf("\tMOV A, %s\n", complete(false, p2));
		printf("\tSUBB A, %s\n", complete(false, p1));
		break;
	case LE:
		printf("\tMOV A, %s\nINC A\n", complete(false, p1));
		printf("\tSUBB A, %s\n", complete(false, p2));
		break;
	case GE:
		printf("\tMOV A, %s\nINC A\n", complete(false, p2));
		printf("\tSUBB A, %s\n", complete(false, p1));
		break;
	case NE:
		printf("\tMOV A, %s\n", complete(false, p2));
		printf("\tSUBB A, %s\n", complete(false, p1));
		printf("\tCLR C\n\tJZ $+3\n\tSETB C\n");
		break;
	case EQ:
		printf("\tMOV A, %s\n", complete(false, p2));
		printf("\tSUBB A, %s\n", complete(false, p1));
		printf("\tCLR C\n\tJNZ $+3\n\tSETB C\n");
		break;
	}
	printf("\tLJMP L%03d\n", lbl2 = lbl++);
	printf("L%03d:\n", lbl3 = lbl++);
	return 0;
}

void adjustBit(int pos) {
	int i;
	for (i = 0; i < pos; i++) {
		printf("\tRR A\n");
	}
	printf("\tAND A, #1\n");
}

int checkbitcondition(nodeType *p1, nodeType *logic, nodeType *p2) {
	printf("L%03d:\n\tCLR C\n", lbl1 = lbl++);

	switch (p2->type) {
	case typeReg:
		printf("\tMOV A, %c%c\n", p2->regId.ireg[0], p2->regId.ireg[1]);
		adjustBit(p2->regId.ireg[3] - '0');
		printf("\tMOV B, A\n");
		break;
	case typePort:
		printf("\tMOV A, %c%c\n", p2->portId.iport[0], p2->portId.iport[1]);
		adjustBit(p2->portId.iport[3] - '0');
		printf("\tMOV B, A\n");
		break;
	case typeCon:
		printf("\tMOV B, #%d\n", p2->con.value);
		break;
	}

	switch (p1->type) {
	case typeReg:
		printf("\tMOV A, %c%c\n", p1->regId.ireg[0], p1->regId.ireg[1]);
		adjustBit(p1->regId.ireg[3] - '0');
		break;
	case typePort:
		printf("\tMOV A, %c%c\n", p1->portId.iport[0], p1->portId.iport[1]);
		adjustBit(p1->portId.iport[3] - '0');
		break;
	case typeCon:
		printf("\tMOV A, #%d\n", p1->con.value);
		break;
	}

	printf("\tSUBB A, B\n");
	switch (logic->opr.oper) {
	case NE:
		printf("\tCLR C\n\tJZ $+3\n\tSETB C\n");
		break;
	case EQ:
		printf("\tCLR C\n\tJNZ $+3\n\tSETB C\n");
		break;
	}
	printf("\tLJMP L%03d\n", lbl2 = lbl++);
	printf("L%03d:\n", lbl3 = lbl++);

	return 0;
}

int assign(nodeType *p1, nodeType *p2) {
	printf("\tMOV %s,", complete(false, p1));
	printf(" %s\n", complete(false, p2));
	return 0;
}

int assignbit(nodeType *p1, nodeType *p2) {
	switch (p2->type) {
	case typeCon:
		if (p2->con.value == 0) {
			printf("\tCLR %s\n", complete(true, p1));
		} else {
			printf("\tSETB %s\n", complete(true, p1));
		}
		break;
	case typeReg:
		printf("\tJB %s, $+3\n", p2->regId.ireg);
		printf("\tCLR %s\n", complete(true, p1));
		printf("\tSJMP $+1\n");
		printf("\tSETB %s\n", complete(true, p1));
		break;
	case typePort:
		printf("\tJB %s, $+3\n", p2->portId.iport);
		printf("\tCLR %s\n", complete(true, p1));
		printf("\tSJMP $+1\n");
		printf("\tSETB %s\n", complete(true, p1));
		break;
	}
	return 0;
}

int arithmatic(nodeType *p, nodeType *p1, nodeType *op, nodeType *p2) {

	printf("\tMOV A, %s\n", complete(false, p1));

	switch (op->opr.oper) {
	case '+':
		printf("\tADD A, %s\n", complete(false, p2));
		printf("\tMOV %s, A\n", complete(false, p));
		break;
	case '-':
		printf("\tSUBB A, %s\n", complete(false, p2));
		printf("\tMOV %s, A\n", complete(false, p));
		break;
	case '*':
		printf("\tMOV B, %s\n", complete(false, p2));
		printf("\tMUL AB");
		printf("\tMOV %s, A\n", complete(false, p));
		break;
	case '/':
		printf("\tMOV B, %s\n", complete(false, p2));
		printf("\tDIV AB");
		printf("\tMOV %s, A\n", complete(false, p));
		break;
	}
	return 0;
}
