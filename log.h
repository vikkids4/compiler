#include <stdio.h>
#include <string.h>

typedef struct{
	char log[1000];
} parserLog;

typedef struct{
	char error[1000];
    char lineno[1000];
} errorLog;

int phaseCount = 0;
int parserLogNo = 0;
int errorLogNo = -1;
parserLog parserLogs[500];
errorLog errorLogs[500];

FILE *logFilePtr;

// Get errors count
int getErrorsCount() {
    return errorLogNo + 1;
}

// Printing parser logs
void addParserLog(char *log) {
    if(parserLogNo > 500) {
        exit(0);
    }
    strcpy(parserLogs[parserLogNo].log, log);
    parserLogNo++;
}

// Printing error logs
void printParseLogs() {
    printInfo("Syntax Analysis");
    int i;
    for(i=0; i < parserLogNo; i++) {
        // print on console
        printf("[%d] %s\n", i, parserLogs[i].log);
        // print on file
        logFilePtr = fopen("log.txt", "a");
        if (logFilePtr == NULL) {
            printf("[ERROR] Token Output File Not Found!");
            exit(1);
        }
        fprintf(logFilePtr, "[%d] %s\n", i, parserLogs[i].log);
        fclose(logFilePtr);
	}
}

// Printing the compiler phases
void printPhase(char *phase) {
    printf("\n\n==============================================================\n\n");
    printf("[PHASE - %d] %s", ++phaseCount, phase);
    printf("\n\n==============================================================\n\n"); 
}

// Printing Info logs
void printInfo(char *info) {
    printf("\n-----------------------------------------------------------------------------------------\n");
    printf("%s", info);
    printf("\n-----------------------------------------------------------------------------------------\n\n");
}

// Storing overall error logs
void addErrorLog(char *error, char *lineno) {
    strcpy(errorLogs[++errorLogNo].error, error);
    strcpy(errorLogs[errorLogNo].lineno, lineno);
    // errorLogNo++;
}

// Printing error logs
void printErrorLogs() {
    printInfo("Error Logs");
    
    int i;
    for(i=0; i < errorLogNo + 1; i++) {
        printf("[%d] %s in line number %s", i, errorLogs[i].error, errorLogs[i].lineno);
	}

    printf("-----------------------------------------------------------------------------------------\n\n\n");
}