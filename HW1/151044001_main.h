/* Include(s) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>



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
#define TAG_IMAGE_PIXEL_WIDTH 256
#define TAG_IMAGE_PIXEL_LENGTH 257
#define TAG_COMPRESSION_TYPE 259
#define TAG_STRIP_OFFSETS 273
#define TAG_ROWS_PER_STRIP 278
#define TAG_STRIP_BYTE_COUNTS 279
#define SIZE_FILE_HEADER 8
#define SIZE_TAG_COUNT 2
#define SIZE_TAG 12
#define TAG_TYPE_3 3
#define TAG_TYPE_4 4
#define FOUR_BYTES 4



/* Structs to transport more data with less effort */
/* Struct for image file headers, total of 8 byte */
struct ImageFileHeader {
    unsigned short byteOrder;
    unsigned short tiffVersion;
    unsigned int ifdOffset;
};

/* Struct for image file directories, total of 12 byte */
struct ImageFileDirectory {
    unsigned short tagCount;
    struct TiffTag *tags;
};

/* Struct for tag fields */
struct TiffTag {
    unsigned short tagID;
    unsigned short dataType;
    unsigned int dataCount;
    unsigned int dataOffset;
};

/* Struct for required image informations */
struct ImageInformations {
    int pixelWidth;
    int pixelHeight;
    int compressionType;
};



/* Function prototype(s) */
int ScanAndPrint(char fileName[]);
int CheckTiffFile(char fileName[], int *fileDescriptor);
int ScanHeaderAndDirectory(int fileDescriptor, struct ImageFileHeader *ifhPtr, struct ImageFileDirectory *ifdPtr);
int ScanImageBitmap(int fileDescriptor, struct ImageFileDirectory ifd, int ***bitmapArray);
void ClearMemoryAllocations(void *, ...);

int GetByteCountByDataType(unsigned short type);
char *GetFileExtension(char fileName[]);

void DebugPrintTag(struct TiffTag tag);

