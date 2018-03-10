/* Include(s) */
#include "stdio.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "errno.h"
#include "string.h"



/* Macro(s) to use as constant definition(s) */
#define ZERO 0
#define TRUE 1
#define FALSE 0
#define REQUIRED_ARGUMENT_COUNT 2
#define EXE_NAME_INDEX 0
#define FILE_NAME_INDEX 1
#define ERROR_EXIT_CODE 1
#define SAFE_EXIT_CODE 0
#define ERROR_CODE -1
#define EXTENSION_LENGTH 4
#define TIF_EXTENSION ".tif"
#define BUFFER_LENGTH 128



/* Function prototype(s) */
int ScanAndPrint(char fileName[]);
int CheckFile(char fileName[]);
int ScanFile(char fileName[]);



int main(int argc, char *argv[]) {
    /* Checking argument count to print usage in case */
    if (argc != REQUIRED_ARGUMENT_COUNT) {
        printf("Usage: %s filename.tif\n", argv[EXE_NAME_INDEX]);
    }
    
    else {
        ScanAndPrint(argv[FILE_NAME_INDEX]);
    }

    return SAFE_EXIT_CODE;
}



/* Main function to scan given file and print to console */
int ScanAndPrint(char fileName[]) {
    /* Checking file first */
    printf("Controlling file \"%s\"\n", fileName);
    if (CheckFile(fileName) == ERROR_CODE) {
        return ERROR_EXIT_CODE;
    }



    return SAFE_EXIT_CODE;
}



/* Function to check if file exists and safe to read */
int CheckFile(char fileName[]) {
    struct stat fileStats;
    int file = ZERO;
    int status = ZERO;
    int nameLength = ZERO;
    int i;


    /* Checking file name if it's extension is .tif */
    nameLength = strlen(fileName);
    if (nameLength > EXTENSION_LENGTH) {
        for (i=0; i<EXTENSION_LENGTH; ++i) {
            if (fileName[nameLength-i-1] != TIF_EXTENSION[EXTENSION_LENGTH-i-1]) {
                fprintf(stderr, "\nError!\nFile type is not supported: %s\nExpected extension: %s\n", fileName, TIF_EXTENSION);
                return ERROR_CODE;
            }
        }
    } else {
        fprintf(stderr, "\nError!\nFile name is too short to be a TIFF file: %s\n", fileName);
        return ERROR_CODE;
    }
    
    /* Checking if path points to a file */
    status = stat(fileName, &fileStats);
    if (status == ERROR_CODE || !S_ISREG(fileStats.st_mode)) {
        fprintf(stderr, "\nError!\nEither path is not a file or couldn't read path stats: %s\n", fileName);
        return ERROR_CODE;
    }

    /* Checking if file can be opened */
    file = open(fileName, O_RDONLY);
    if (file == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't open file to check: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }

    /* Closing file to release it */
    status = close(file);
    if (status == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't close file after check: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }


    return SAFE_EXIT_CODE;
}



/* Function to read information from file */
int ScanFile(char fileName[]) {
    int file = ZERO;
    int status = ZERO;
    char buffer[BUFFER_LENGTH];


    file = open(fileName, O_RDONLY);
    if (file == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't open file to scan: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }



    status = close(file);
    if (status == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't close file after scan: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }


    return SAFE_EXIT_CODE;
}
