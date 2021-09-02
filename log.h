#include <stdio.h>
#include <string.h>

typedef struct{
	char error[1000];
    char lineno[1000];
} errorLog;

int phaseCount = 0;
int errorLogNo = 0;
errorLog errorLogs[500];

void printPhase(char *phase) {
    printf("\n\n==============================================================\n\n");
    printf("[PHASE - %d] %s", ++phaseCount, phase);
    printf("\n\n==============================================================\n\n"); 
}

void printInfo(char *info) {
    printf("\n\n==============================================================\n\n");
    printf("[INFOs] %s", info);
    printf("\n\n==============================================================\n\n");
}

void addErrorLog(char *error, char *lineno) {
    strcpy(errorLogs[errorLogNo].error, error);
    strcpy(errorLogs[errorLogNo].lineno, lineno);
    errorLogNo++;
}

void printErrorLogs() {
    printf("\n\n##############################################################\n\n");
    printf("Error Logs");
    printf("\n\n##############################################################\n\n");
    
    int i;
    for(i=0; i < errorLogNo; i++) {
        printf("[%d] %s in line number %s", i, errorLogs[i].error, errorLogs[i].lineno);
	}

    printf("\n##############################################################\n\n\n\n");
}