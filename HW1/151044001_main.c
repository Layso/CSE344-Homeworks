/* Include(s) */
#include "stdio.h"
#include "sys/stat.h"
#include "sys/types.h"
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
#define SAFE_CODE 0
#define TIF_EXTENSION_1 "tif"
#define TIF_EXTENSION_2 "TIF"
#define TIF_EXTENSION_3 "tiff"
#define TIF_EXTENSION_4 "TIFF"
#define BUFFER_LENGTH 128
#define NULL_CHARACTER '\0'
#define INTEL_BYTE_ORDER "II"
#define MOTOROLA_BYTE_ORDER "MM"



/* Structs transport more data with less effort */
struct ImageFileHeader {
    unsigned short byteOrder;
    unsigned short tiffVersion;
    unsigned int ifdOffset;
};



struct ImageFileDirectory {
    unsigned short tagID;
    unsigned short dataType;
    unsigned int dataCount;
    unsigned int dataOffset;
};



/* Function prototype(s) */
int ScanAndPrint(char fileName[]);
int CheckFile(char fileName[]);
int ScanFileHeader(char fileName[], struct ImageFileHeader *ifhPtr);
int ScanFileDirectory(char fileName[], struct ImageFileHeader ifh, int *pixelWidth, int *pixelHeight, int *layerCount);
char *GetFileExtension(char fileName[]);



int main(int argc, char *argv[]) {
    /* Checking argument count to print usage in case */
    if (argc != REQUIRED_ARGUMENT_COUNT) {
        printf("Usage: %s filename\n", argv[EXE_NAME_INDEX]);
        return ERROR_EXIT_CODE;
    }
    
    else if (ScanAndPrint(argv[FILE_NAME_INDEX]) == ERROR_CODE) {
        fprintf(stderr, "Quitting program, please try again after fixing the error\n");
        return ERROR_EXIT_CODE;
    }


    return SAFE_EXIT_CODE;
}



/* Main function to scan given file and print to console */
int ScanAndPrint(char fileName[]) {
    struct ImageFileHeader ifh;
    int pixelHeight;
    int pixelWidth;
    int layerCount;


    /* Clearing struct */
    memset(&ifh, ZERO, sizeof(ifh));


    /* Checking file first */
    printf("Controlling file: \"%s\"\n", fileName);
    if (CheckFile(fileName) == ERROR_CODE) {
        return ERROR_CODE;
    }

    /* Scanning image file header */
    printf("Scanning file header\n");
    if (ScanFileHeader(fileName, &ifh) == ERROR_CODE) {
        return ERROR_CODE;
    }

    /* Scanning image file directory */
    printf("Scanning file directory\n");
    if (ScanFileDirectory(fileName, ifh, &pixelWidth, &pixelHeight, &layerCount)) {
        return ERROR_CODE;
    }

    printf("Byte order: %04X\n", ifh.byteOrder);
    printf("Version: %04X\n", ifh.tiffVersion);
    printf("Offset: %04X\n", ifh.ifdOffset);
   /* if (!strcmp(INTEL_BYTE_ORDER, ifh.byteOrder))
        printf("Intel\n");
    else if (!strcmp(MOTOROLA_BYTE_ORDER, ifh.byteOrder))
        printf("Motorola\n");
    else
        printf("Undefined\n");
*/
    return SAFE_EXIT_CODE;
}



/* Function to check if file exists and safe to read */
int CheckFile(char fileName[]) {
    int file = ZERO;
    int status = ZERO;
    struct stat fileStats;
    char *fileExtension = NULL;


    fileExtension = GetFileExtension(fileName);
    /* Checking file name if it's extension is .tif */
    if ((strcmp(fileExtension, TIF_EXTENSION_1) != ZERO) && (strcmp(fileExtension, TIF_EXTENSION_2) != ZERO)
     && (strcmp(fileExtension, TIF_EXTENSION_3) != ZERO) && (strcmp(fileExtension, TIF_EXTENSION_4) != ZERO)) {
        fprintf(stderr, "\nError!\nFile extension is incompatible: %s\nExpected extensions: .%s / .%s / .%s / .%s\n", 
        fileName, TIF_EXTENSION_1, TIF_EXTENSION_2, TIF_EXTENSION_3, TIF_EXTENSION_4);
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
int ScanFileHeader(char fileName[], struct ImageFileHeader *ifhPtr) {
    int file = ZERO;
    int status = ZERO;
    int byteCount = ZERO;
    int flag = TRUE;


    /* Opening file to read */
    file = open(fileName, O_RDONLY);
    if (file == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't open file to scan file header: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }
    
    /* Reading file header informations to IFH pointer */
    byteCount = read(file, &(ifhPtr->byteOrder), sizeof(ifhPtr->byteOrder));
    if (byteCount == ERROR_CODE || byteCount == ZERO) {
        fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading byte order from file header: %s\nReason of error: %s\n", fileName, strerror(errno));
        flag = FALSE;
    }
    
    if (flag) {
        byteCount = read(file, &(ifhPtr->tiffVersion), sizeof(ifhPtr->tiffVersion));
        if (byteCount == ERROR_CODE || byteCount == ZERO) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading tiff version from file header: %s\nReason of error: %s\n", fileName, strerror(errno));
            flag = FALSE;
        }   
    }

    if (flag) {
        byteCount = read(file, &(ifhPtr->ifdOffset), sizeof(ifhPtr->ifdOffset));
        if (byteCount == ERROR_CODE || byteCount == ZERO) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading ifd offset from file header: %s\nReason of error: %s\n", fileName, strerror(errno));
            flag = FALSE;
        }   
    }
    
    /* Closing file */
    status = close(file);
    if (status == ERROR_CODE ) {
        fprintf(stderr, "\nError!\nCouldn't close file after file header scan: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }
    

    /* Return error code if an error occured during reading */
    if (!flag) {
        return ERROR_CODE;
    }


    return SAFE_EXIT_CODE;
}



/* Function to read image file directory from file */
int ScanFileDirectory(char fileName[], struct ImageFileHeader ifh, int *pixelWidth, int *pixelHeight, int *layerCount) {
    int i;
    int file;
    int status;
    int flag = TRUE;
    unsigned short tagCount;
    unsigned int nextIfd;
    struct ImageFileDirectory ifd;

    memset(&ifd, ZERO, sizeof(ifd));

    /* Opening file to read */
    file = open(fileName, O_RDONLY);
    if (file == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't open file to scan file directory: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }

    /*ifh.ifdOffset = (ifh.ifdOffset >> 8) || (ifh.ifdOffset << 8);*/

    /* Changing file cursor position to IFD offset */
    if (lseek(file, ifh.ifdOffset, SEEK_SET) == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't change the cursor position to IFD offset for file: %s\nReason of error: %s\n", fileName, strerror(errno));
        flag = FALSE;
    }

    read(file, &tagCount, sizeof(tagCount));
    
    for (i=0; i<tagCount; ++i) {
        read(file, &ifd, sizeof(ifd));
        printf("Tag ID: %d\nData Type: %d\nData Count: %d\nData Offset: %d\n\n", ifd.tagID, ifd.dataType, ifd.dataCount, ifd.dataOffset);
    }

    read (file, &nextIfd, sizeof(nextIfd));
    printf("Current offset: %d\nNext IFD offset: %d\n", lseek(file, 0, SEEK_CUR), nextIfd);

    /* Closing file */
    status = close(file);
    if (status == ERROR_CODE ) {
        fprintf(stderr, "\nError!\nCouldn't close file after file directory scan: %s\nReason of error: %s\n", fileName, strerror(errno));
        return ERROR_CODE;
    }


    return SAFE_EXIT_CODE;
}



/* Code snippet from https://stackoverflow.com/questions/5309471/getting-file-extension-in-c to find the file extension */
char *GetFileExtension(char fileName[]) {
    char *dot = strrchr(fileName, '.');
    if(!dot || dot == fileName) return "";
    return dot + 1;
}
