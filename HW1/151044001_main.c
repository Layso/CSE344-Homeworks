#include "151044001_main.h"



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
    struct ImageFileDirectory ifd;
    int fileDescriptor;
    int errorFlag = FALSE;
    int **bitmapArray;


    /* Checking file first */
    errorFlag = CheckTiffFile(fileName, &fileDescriptor);
    
    /* Scanning image file header */
    if (!errorFlag) {
        errorFlag = ScanHeaderAndDirectory(fileDescriptor, &ifh, &ifd);
    }

    /* Scanning image bitmap */
    if (!errorFlag) {
        errorFlag = ScanImageBitmap(fileDescriptor, ifd, &bitmapArray);
    }



    ClearMemoryAllocations(&fileDescriptor, &ifd, &bitmapArray);

    return errorFlag;
}



/* Function to check if file exists and safe to read */
int CheckTiffFile(char fileName[], int *fileDescriptor) {
    int errorFlag = FALSE;
    struct stat fileStats;
    char *fileExtension = NULL;
    int status;

    
    /* Checking if file extension belongs to a TIFF file */
    fileExtension = GetFileExtension(fileName);
    if ((strcmp(fileExtension, TIF_EXTENSION_1) != ZERO) && (strcmp(fileExtension, TIF_EXTENSION_2) != ZERO)
     && (strcmp(fileExtension, TIF_EXTENSION_3) != ZERO) && (strcmp(fileExtension, TIF_EXTENSION_4) != ZERO)) {
        fprintf(stderr, "\nError!\nFile extension is incompatible: %s\nExpected extensions: .%s / .%s / .%s / .%s\n", 
        fileName, TIF_EXTENSION_1, TIF_EXTENSION_2, TIF_EXTENSION_3, TIF_EXTENSION_4);
        errorFlag = TRUE;
    }
    
    /* Checking if path points to a file */
    if (!errorFlag) {
        status = stat(fileName, &fileStats);
        if (status == ERROR_CODE || !S_ISREG(fileStats.st_mode)) {
            fprintf(stderr, "\nError!\nEither path is not a file or couldn't read path stats: %s\n", fileName);

            if (status == ERROR_CODE)
                fprintf(stderr, "Reason of error: %s\n", strerror(errno));
            
            errorFlag = TRUE;
        }
    }

    /* Checking if file can be opened */
    if (!errorFlag) {
        *fileDescriptor = open(fileName, O_RDONLY);
        if (*fileDescriptor == ERROR_CODE) {
            fprintf(stderr, "\nError!\nCouldn't open file to check: %s\nReason of error: %s\n", fileName, strerror(errno));
            errorFlag = TRUE;
        }
    }
    

    return errorFlag;
}



/* Function to read information from file */
int ScanHeaderAndDirectory(int fileDescriptor, struct ImageFileHeader *ifhPtr, struct ImageFileDirectory *ifdPtr) {
    int byteCount = ZERO;
    int errorFlag = FALSE;
    int i;

    
    /* Reading file header informations to IFH pointer */
    byteCount = read(fileDescriptor, ifhPtr, sizeof(*ifhPtr));
    if (byteCount == ERROR_CODE || byteCount != SIZE_FILE_HEADER) {
        fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading file header\nReason of error: %s\n", strerror(errno));
        errorFlag = TRUE;
    }

    /* Changing file cursor position to first IFD offset */
    if (!errorFlag) {
        if (lseek(fileDescriptor, ifhPtr->ifdOffset, SEEK_SET) == ERROR_CODE) {
            fprintf(stderr, "\nError!\nCouldn't change the cursor position to IFD offset\nReason of error: %s\n", strerror(errno));
            errorFlag = TRUE;
        }
    }

    /* Reading number of tags from directory */
    if (!errorFlag) {
        byteCount = read(fileDescriptor, &(ifdPtr->tagCount), sizeof(ifdPtr->tagCount));
        if (byteCount == ERROR_CODE || byteCount != SIZE_TAG_COUNT) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading number of tags\nReason of error: %s\n", strerror(errno));
            errorFlag = TRUE;
        }
    }

    /* Allocating memory for the image tags */
    if (!errorFlag) {
        ifdPtr->tags = (struct TiffTag*)calloc(ifdPtr->tagCount, sizeof(struct TiffTag));
        if (ifdPtr->tags == NULL) {
            fprintf (stderr, "\nError!\nMemory allocation for image tags are failed\n");
            errorFlag = TRUE;
        }
    }

    /* Reading provided number of tags to allocated memory */
    if (!errorFlag) {
        for (i=0; i<ifdPtr->tagCount && !errorFlag; ++i) {
            byteCount = read(fileDescriptor, (ifdPtr->tags)+i, sizeof(struct TiffTag));
            if (byteCount == ERROR_CODE || byteCount != SIZE_TAG) {
                fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during readingasd number of tags\nReason of error: %s\n", strerror(errno));
                errorFlag = TRUE;
            }
        }
    }


    return errorFlag;
}



/* Function to read image data and load to 2D array */
int ScanImageBitmap(int fileDescriptor, struct ImageFileDirectory ifd, int ***bitmapArray) {
    struct TiffTag currentTag;
    unsigned int valueOfTag;
    unsigned int imageLength;
    unsigned int imageWidth;
    unsigned int rowsPerStrip;
    unsigned int stripOffsets;
    unsigned int stripByteCounts;
    unsigned int bitmapData;
    off_t currentCursor;
    int stripsPerImage;
    int i;
    char character;


    for (i=0; i<ifd.tagCount; ++i) {
        currentTag = ifd.tags[i];
        if (currentTag.dataCount * GetByteCountByDataType(currentTag.dataType) <= FOUR_BYTES) {
            valueOfTag = currentTag.dataOffset;
        }

        else {
            /* TODO: Error Check */
            currentCursor = lseek(fileDescriptor, ZERO, SEEK_CUR);
            lseek(fileDescriptor, currentTag.dataOffset, SEEK_SET);
            read(fileDescriptor, &valueOfTag, sizeof(valueOfTag));    
            lseek(fileDescriptor, currentCursor, SEEK_SET);
        }


        switch (currentTag.tagID) {
            case TAG_IMAGE_PIXEL_WIDTH:
                imageWidth = valueOfTag;
                break;
            
            case TAG_IMAGE_PIXEL_LENGTH:
                imageLength = valueOfTag;
                break;

            case TAG_ROWS_PER_STRIP:
                DebugPrintTag(currentTag);
                rowsPerStrip = valueOfTag;
                break;
            
            case TAG_STRIP_OFFSETS:
                DebugPrintTag(currentTag);
                stripOffsets = valueOfTag;
                break;
            
            case TAG_STRIP_BYTE_COUNTS:
                DebugPrintTag(currentTag);
                stripByteCounts = valueOfTag;
                break;

            default:
                break;
        }
    }

    /* Formula to find strips per image, taken from tiff documentation itself */
    stripsPerImage = (int)((imageLength + rowsPerStrip -1) / rowsPerStrip);
    printf("Image Length: %d\nImage Width: %d\nRows P Strip: %d\nStrips P Image: %d\nStrip Offsets: %d\nStrip Byte Counts: %d\n", imageLength, imageWidth, rowsPerStrip, stripsPerImage, stripOffsets, stripByteCounts);

    
    return 0;
}



/* Function to deallocate memory and close descriptors */
void ClearMemoryAllocations(void *param, ...) {
    /* TODO: deallocate dynamically allocated variables
             close descriptors
    */
}



/* Code snippet from https://stackoverflow.com/questions/5309471/getting-file-extension-in-c to find the file extension */
char *GetFileExtension(char fileName[]) {
    char *dot = strrchr(fileName, '.');
    if(!dot || dot == fileName) return "";
    return dot + 1;
}


int GetByteCountByDataType(unsigned short type) {
    switch (type) {
        case TAG_TYPE_3:
            return sizeof(unsigned short);
        
        case TAG_TYPE_4:
            return sizeof(unsigned int);

        default:
            break;
    }

    return ERROR_CODE;
}



void DebugPrintTag(struct TiffTag tag) {
    printf("Tag ID: %d\nData Type: %d\nData Count: %d\nData Offset: %d\n\n", tag.tagID, tag.dataType, tag.dataCount, tag.dataOffset);
}