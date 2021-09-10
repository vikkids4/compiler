#include <stdio.h>
#include <string.h>

typedef struct{
	char error[1000];
    char lineno[1000];
} errorLog;

int phaseCount = 0;
int errorLogNo = 0;
errorLog errorLogs[500];

FILE *logFilePtr;
int parserLogLine = 0;

// Printing parser logs
void printParserLog(char *log) {
    if(parserLogLine > 500) {
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

// Printing the compiler phases
void printPhase(char *phase) {
    printf("\n\n==============================================================\n\n");
    printf("[PHASE - %d] %s", ++phaseCount, phase);
    printf("\n\n==============================================================\n\n"); 
}

// Printing Info logs
void printInfo(char *info) {
    printf("\n\n==============================================================\n\n");
    printf("[INFOs] %s", info);
    printf("\n\n==============================================================\n\n");
}

// Storing overall error logs
void addErrorLog(char *error, char *lineno) {
    strcpy(errorLogs[errorLogNo].error, error);
    strcpy(errorLogs[errorLogNo].lineno, lineno);
    errorLogNo++;
}

// Printing error logs
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