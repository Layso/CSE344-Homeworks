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
    int swapByteOrder;


    /* Checking file first */
    errorFlag = CheckTiffFile(fileName, &fileDescriptor, &ifh, &swapByteOrder);
    
    /* Scanning image file header */
    if (!errorFlag) {
        errorFlag = ScanFileDirectory(fileDescriptor, &ifh, &ifd, &swapByteOrder);
    }

    /* Scanning image bitmap */
    if (!errorFlag) {
        errorFlag = ScanImageBitmap(fileDescriptor, ifd, swapByteOrder);
    }

    if (!errorFlag) {
        free(ifd.tags);
        close(fileDescriptor);
    }

    return errorFlag;
}



/* Function to check if file exists and safe to read */
int CheckTiffFile(char fileName[], int *fileDescriptor, struct ImageFileHeader *ifhPtr, int *swapByteOrder) {
    int byteCount = ZERO;
    int errorFlag = FALSE;


    /* Checking if file can be opened */
    *fileDescriptor = open(fileName, O_RDONLY);
    if (*fileDescriptor == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't open file to check: %s\nReason of error: %s\n", fileName, strerror(errno));
        errorFlag = TRUE;
    }

    /* Reading file header */   
    if (!errorFlag) {
        byteCount = read(*fileDescriptor, ifhPtr, sizeof(*ifhPtr));
        if (byteCount == ERROR_CODE || byteCount != SIZE_FILE_HEADER) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading file header\nReason of error: %s\n", strerror(errno));
            errorFlag = TRUE;
        }
    }
    
    /* Validating the tiff file */
    if (!errorFlag) {
        *swapByteOrder = GetEndiannessByType(ifhPtr->byteOrder);
        if (*swapByteOrder == ERROR_CODE) {
            fprintf (stderr, "\nError!\nFile structure doesn't match with TIFF standarts\n");
            errorFlag = TRUE;
        }
    }

    if (!errorFlag) {
        if (*swapByteOrder) {
            Swap2Bytes(&ifhPtr->tiffVersion);
            Swap4Bytes(&ifhPtr->ifdOffset);
        }

        if (ifhPtr->tiffVersion != MEANING_OF_LIFE) {
            fprintf (stderr, "\nError!\nFile structure doesn't match with TIFF standarts\n");
            errorFlag = TRUE;
        }
    }


    return errorFlag;
}



/* Function to read information from file */
int ScanFileDirectory(int fileDescriptor, struct ImageFileHeader *ifhPtr, struct ImageFileDirectory *ifdPtr, int *byteOrderStatus) {
    int byteCount = ZERO;
    int errorFlag = FALSE;
    int i;


    /* Changing file cursor position to first IFD offset */
    if (lseek(fileDescriptor, ifhPtr->ifdOffset, SEEK_SET) == ERROR_CODE) {
        fprintf(stderr, "\nError!\nCouldn't change the cursor position to IFD offset\nReason of error: %s\n", strerror(errno));
        errorFlag = TRUE;
    }

    /* Reading number of tags from directory */
    if (!errorFlag) {
        byteCount = read(fileDescriptor, &(ifdPtr->tagCount), sizeof(ifdPtr->tagCount));
        if (byteCount == ERROR_CODE || byteCount != SIZE_TAG_COUNT) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading number of tags\nReason of error: %s\n", strerror(errno));
            errorFlag = TRUE;
        }

        if (*byteOrderStatus) {
            Swap2Bytes(&ifdPtr->tagCount);
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
                fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading tags\nReason of error: %s\n", strerror(errno));
                errorFlag = TRUE;
            }
            
            if (*byteOrderStatus) {
                /*
                printf("ID:     %04x\n", ifdPtr->tags[i].tagID);
                printf("Type:   %04x\n", ifdPtr->tags[i].dataType);
                printf("Count:  %08x\n", ifdPtr->tags[i].dataCount);
                printf("Offset: %08x\n\n", ifdPtr->tags[i].dataOffset);
                */

                Swap2Bytes(&ifdPtr->tags[i].tagID);
                Swap2Bytes(&ifdPtr->tags[i].dataType);
                Swap4Bytes(&ifdPtr->tags[i].dataCount);
                Swap4Bytes(&ifdPtr->tags[i].dataOffset);

                /*
                printf("ID:     %04x\n", ifdPtr->tags[i].tagID);
                printf("Type:   %04x\n", ifdPtr->tags[i].dataType);
                printf("Count:  %08x\n", ifdPtr->tags[i].dataCount);
                printf("Offset: %08x\n\n\n\n\n\n", ifdPtr->tags[i].dataOffset);
                */
            }
        }
    }


    return errorFlag;
}



/* Function to read image data and load to 2D array */
int ScanImageBitmap(int fileDescriptor, struct ImageFileDirectory ifd, int byteOrderStatus) {
    struct TiffTag tag;
    unsigned int *data = NULL;
    unsigned int *offsets = NULL;
    unsigned int *offsetBytes = NULL;
    unsigned int compression;
    unsigned int offsetCount;
    unsigned int bitsPerSample;
    unsigned int samplePerPixel = 1;
    unsigned char *strips = NULL;
    int imageLength;
    int imageWidth;
    int index;
    int totalStripByte = 0;
    int whiteIs;
    int bitPerPixel;
    int byteCount;
    int status;
    int errorFlag = FALSE;
    int i, j;


    /* Reading data from tags */
    for (i=0; i<ifd.tagCount && !errorFlag; ++i) {
        tag = ifd.tags[i];
        data = (unsigned int*)calloc(tag.dataCount, GetByteCountByDataType(tag.dataType));
        switch (tag.tagID) {
            case TAG_IMAGE_PIXEL_LENGTH:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    imageLength = data[ZERO];
                }

                break;

            case TAG_IMAGE_PIXEL_WIDTH: 
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    imageWidth = data[ZERO];
                }

                break;

            case TAG_COMPRESSION_TYPE: 
                DebugPrintTag(tag);
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    compression = (unsigned short)data[ZERO];
                }

                break;
            
            case TAG_BITS_PER_SAMPLE:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    bitsPerSample = (unsigned short)data[ZERO];
                }

                break;

            case TAG_SAMPLE_PER_PIXEL:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    samplePerPixel = (unsigned short)data[ZERO];
                }

                break;
            
            case TAG_PHOTOMETRIC_INTERPRETATION:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                if (errorFlag) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                }

                else {
                    whiteIs = (unsigned short)data[ZERO];
                }

                break;

            case TAG_STRIP_OFFSETS:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                offsets = (unsigned int *)calloc(tag.dataCount, sizeof(unsigned int));

                if (errorFlag || offsets == NULL) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                    errorFlag = TRUE;
                }

                else {
                    offsetCount = tag.dataCount;
                    for (j=0; j<tag.dataCount; ++j)
                        offsets[j] = data[j];
                }
                
                break;
            
            case TAG_STRIP_BYTE_COUNTS:
                errorFlag = ReadFieldData(fileDescriptor, tag, byteOrderStatus, data);
                offsetBytes = (unsigned int *)calloc(tag.dataCount, sizeof(unsigned int));

                if (errorFlag || offsetBytes == NULL) {
                    fprintf (stderr, "\nError!\nCouldn't read tag value from file\nTag number: %d\n", tag.tagID);
                    errorFlag = TRUE;
                }
                
                else {
                    offsetCount = tag.dataCount;
                    for (j=0; j<tag.dataCount; ++j)
                        offsetBytes[j] = data[j];
                }

                break;
        }
        
        free(data);
    }

    /* Checking compression */
    if (compression != NO_COMPRESSION) {
        printf("\nError!\nThis reader (unfortunately) does not support compressed image data: %d\n", compression);
        errorFlag = TRUE;
    }

    /* Required calculations to read image data */
    if (!errorFlag) {
        bitPerPixel = bitsPerSample * samplePerPixel;
        for (i=0; i<offsetCount; ++i) {
            totalStripByte += offsetBytes[i];
        }
    }

    /* Reading image bitmap from file */
    if (!errorFlag) {
        index = ZERO;
        strips = (unsigned char*)calloc(totalStripByte, sizeof(char));
        for (i=0; i<offsetCount && !errorFlag; ++i) {
            status = lseek(fileDescriptor, offsets[i], SEEK_SET);
            if (status == ERROR_CODE) {
                fprintf (stderr, "\nError!\nEither end of file reached or an error has occured while seeking over data\nReason of error: %s\n", strerror(errno));
                errorFlag = TRUE;
            }

            for (j=0; j<offsetBytes[i] && !errorFlag; ++j) {
                byteCount = read(fileDescriptor, &strips[index++], sizeof(unsigned char));
                
                if (byteCount == ERROR_CODE || byteCount != sizeof(unsigned char)) {
                    fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading bitmap data\nReason of error: %s\n", strerror(errno));
                    errorFlag = TRUE;
                }
            }
        }
    }

    /* Printing image with informations */
    if (!errorFlag) {
        printf("Width: %d\n", imageWidth);
        printf("Length: %d\n", imageLength);
        printf("Byte order: %s\n", byteOrderStatus == 0 ? "II" : "MM");
        printf("------------\n");


        index = 0;
        if (byteOrderStatus) {
            printf("\nError!\nThis reader (unfortunately) does not support Motorola byte order which image structure currently depends on\n");
        }

        else if (!(bitPerPixel % SIZE_BYTE)) {
            for (i = 0; i<imageLength; ++i) {
                for (j=0; j<imageWidth; ++j) {
                    if (whiteIs) {
                        if (strips[index] == ZERO)
                            printf("%d ", BLACK);
                        else
                            printf("%d ", WHITE);
                    }

                    else {
                        if (strips[index] == ZERO)
                            printf("%d ", WHITE);
                        else
                            printf("%d ", BLACK);
                    }
                    index += samplePerPixel;
                }

                printf("\n");
            }
        }

        else {
            printf("\nError!\nThis reader (unfortunately) does not support this image due to bit per pixel value: %d\n", bitPerPixel);
        }
    }
    

    /* Releasing allocated memory */
    free(strips);
    free(offsets);
    free(offsetBytes);


    return errorFlag;
}



/* Helper function to read field data from image file */
int ReadFieldData(int fileDescriptor, struct TiffTag tag, int byteOrderStatus, unsigned int *data) {
    int dataSize = ZERO;
    int errorFlag = FALSE;
    int byteCount;
    int status;
    int i;
    unsigned int buffer;


    /* Taking byte size of tag data */
    dataSize = GetByteCountByDataType(tag.dataType);

    /* If field holds data more than offset field has (4 bytes) read value(s) from offset */
    if (dataSize * tag.dataCount > sizeof(tag.dataOffset)) {
        status = lseek(fileDescriptor, tag.dataOffset, SEEK_SET);
        if (status == ERROR_CODE) {
            fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during seeking data\nReason of error: %s\n", strerror(errno));
            errorFlag = TRUE;
        }
        
        if (!errorFlag) {
            for (i=0; i<tag.dataCount && !errorFlag; ++i) {
                byteCount = read(fileDescriptor, &buffer, dataSize);   
                if (byteCount == ERROR_CODE || byteCount != dataSize) {
                    fprintf (stderr, "\nError!\nEither end of file reached or an error has occured during reading tags\nReason of error: %s\n", strerror(errno));
                    errorFlag = TRUE;
                }

                else {
                    data[i] = buffer;
                }
            }
        }
    }
    
    /* Else read directly from offset field */
    else {
        data[ZERO] = tag.dataOffset;
    }
    

    return errorFlag;
}



/* Helper function  */
int GetByteCountByDataType(unsigned short type) {
    switch (type) {
        case TAG_TYPE_1:
            return sizeof(unsigned char);
        
        case TAG_TYPE_2:
            return sizeof(unsigned char);
        
        case TAG_TYPE_3:
            return sizeof(unsigned short);
        
        case TAG_TYPE_4:
            return sizeof(unsigned int);
        
        case TAG_TYPE_5:
            return sizeof(unsigned int);

        default:
            return sizeof(unsigned int);
    }
    

    return ERROR_CODE;
}



/* Helper function to get endian-ness from given file type */
int GetEndiannessByType(unsigned short type) {
    if (type == 0x4D4D)
        return BIG_ENDIAN;
    else if (type == 0x4949)
        return LITTLE_ENDIAN;
    else
        return ERROR_CODE;
}



/* Helper function to print a byte bit by bit */
void PrintByteByBits(unsigned char byte) {
    int byteBits[8];
    unsigned char mask = 1;
    int i;

    for (i=1; i<8; ++i) {
        byteBits[i] = (byte & mask<<i) != 0;
    }
    
    
    for (i=0; i<8; ++i) {
        printf("%d ", byteBits[7-i]);
    }
}



/* Helper function to find system endian-ness
 * Source: https://stackoverflow.com/questions/8571089/how-can-i-find-endian-ness-of-my-pc-programmatically-using-c
 */
int GetEndianness() {
    int num = 1;

    if (*(char *)&num == 1) {
        return LITTLE_ENDIAN;
    }

    else {
        return BIG_ENDIAN;
    }
}



/* Helper function to swap byte order of a 2-byte variable
 * Source: https://stackoverflow.com/questions/2182002/convert-big-endian-to-little-endian-in-c-without-using-provided-func
 */
void Swap2Bytes(unsigned short *num) {
    *num = (*num>>8) | (*num<<8);
}



/* Helper function to swap byte order of a 4-byte variable 
 * Source: https://stackoverflow.com/questions/2182002/convert-big-endian-to-little-endian-in-c-without-using-provided-func
 */
void Swap4Bytes(unsigned int *num) {
    *num =  ((*num>>24)&0xff) | ((*num<<8)&0xff0000) | ((*num>>8)&0xff00) | ((*num<<24)&0xff000000);
}



void DebugPrintTag(struct TiffTag tag) {
    printf("Tag ID: %x\nData Type: %x\nData Count: %x\nData Offset: %08x\n\n", tag.tagID, tag.dataType, tag.dataCount, tag.dataOffset);
}