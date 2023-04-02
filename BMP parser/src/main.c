#include <utils.h>
#include <stdlib.h>
#include <string.h>

// Exercise 1
void print_header(char const *filename) {

    FILE* inputFilePointer = fopen(filename, "rb");
    if (inputFilePointer == NULL)
    {
        printf("Error while opening a file.");
        return;
    }

    // Read the header data
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    printf("BITMAPFILEHEADER:\n");
    printf("  bfType:          0x%04X\n", fileHeader.bfType);
    printf("  bfSize:          %0u\n", fileHeader.bfSize);
    printf("  bfReserved1:     0x%0X\n", fileHeader.bfReserved1);
    printf("  bfReserved2:     0x%0X\n", fileHeader.bfReserved2);
    printf("  bfOffBits:       %0u\n", fileHeader.bfOffBits);

    printf("BITMAPINFOHEADER:\n");
    printf("  biSize:          %0u\n", fileInfoHeader.biSize);
    printf("  biWidth:         %0d\n", fileInfoHeader.biWidth);
    printf("  biHeight:        %0d\n", fileInfoHeader.biHeight);
    printf("  biPlanes:        %0u\n", fileInfoHeader.biPlanes);
    printf("  biBitCount:      %0u\n", fileInfoHeader.biBitCount);
    printf("  biCompression:   %0u\n", fileInfoHeader.biCompression);
    printf("  biSizeImage:     %0u\n", fileInfoHeader.biSizeImage);
    printf("  biXPelsPerMeter: %0d\n", fileInfoHeader.biXPelsPerMeter);
    printf("  biYPelsPerMeter: %0d\n", fileInfoHeader.biYPelsPerMeter);
    printf("  biClrUsed:       %0u\n", fileInfoHeader.biClrUsed);
    printf("  biClrImportant:  %0u\n", fileInfoHeader.biClrImportant);

    fclose(inputFilePointer);
}

// Exercise 2

// TODO: open files separately in every exercise
// First open a file, read the header, info header and the rest
// and close the file

// TODO: First 8 bytes of encoding are used for the number
// describing how many characters you want to encode
// print alerts if more

// transforms the letter into ASCII binary representation
void putToBinary(char letter, char *dest) {
    for (int i = 7; i >= 0; i--) {
        int bit = (letter >> i) & 1;
        dest[7-i] = bit + '0';
    }
    dest[8] = '\0';
}

void steganography(const char *textToEncode, const char* inputFilename) {
    FILE* inputFilePointer = fopen(inputFilename, "rb+");
    if (inputFilePointer == NULL)
    {
        printf("Error while opening a file.");
        return;
    }

    // Read the header data
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    unsigned char *fileFullHeader = malloc(fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader));
    fread(fileFullHeader, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), 1, inputFilePointer);

    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;
    int numLettersLine = fileInfoHeader.biWidth * 3 / 8;
    char letters[strlen(textToEncode) + 1][9];
    putToBinary(strlen(textToEncode), letters[0]); 
    // Put the number as a first thing to encode
    // the number represents the length of a sentence to encode

    for (int i = 1; i < strlen(textToEncode) + 1; i++) {
        putToBinary(textToEncode[i - 1], letters[i]);
    }

    unsigned char pixelsLetters[strlen(textToEncode) + 1][8];
    for (int i = 0; i < strlen(textToEncode) + 1; i++) {
        fseek(inputFilePointer, fileHeader.bfOffBits + (i * 8), SEEK_SET);
        fread(pixelsLetters[i], 8, 1, inputFilePointer);
        
        for (int j = 0; j < 8; j++) {
            if (letters[i][j] == '0') {
                if ((int) pixelsLetters[i][j] % 2 == 1) {
                    pixelsLetters[i][j] -= 1;
                }
            }
            else {
                if ((int) pixelsLetters[i][j] % 2 == 0) {
                    pixelsLetters[i][j] += 1;
                }
            }
        }
    }

    fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET);
    fwrite(pixelsLetters, 8 * (strlen(textToEncode) + 1), 1, inputFilePointer);
    fclose(inputFilePointer);
}

void takeLeastSignBits(unsigned char* input, unsigned char* output) {
    for (int i = 0; i < 8; i++) {
        if (input[i] % 2 == 0) {
            output[i] = '0';
        }
        else {
            output[i] = '1';
        }
    }
}

// TODO: handle the padding while encoding and decoding information
// from the file

void decypher(const char* filename) {
    FILE* inputFilePointer = fopen(filename, "rb");
    if (inputFilePointer == NULL)
    {
        printf("Error while opening a file.");
        return;
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    unsigned char *fileFullHeader = malloc(fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader));
    fread(fileFullHeader, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), 1, inputFilePointer);
    fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET);

    unsigned char messageLengthBinary[8];
    fread(messageLengthBinary, 8, 1, inputFilePointer);
    
    //get the length of the information
    unsigned char output[8];
    takeLeastSignBits(messageLengthBinary, output);
    long int length = strtol(output, NULL, 2);

    unsigned char message[length + 1]; // +1 for '\0' 
    for (int i = 0; i < length; i++) {
        unsigned char messageBinary[8];
        fread(messageBinary, 8, 1, inputFilePointer);

        unsigned char output[8];
        takeLeastSignBits(messageBinary, output);
        long int letter = strtol(output, NULL, 2);
        message[i] = (char) letter;
    }
    message[length] = '\0';
    printf("%s \n", message);

    fclose(inputFilePointer);
}

// TODO: Check first exercise
// TODO: Check second exercise
// TODO: Check thrid exercise
// TODO: Check Fourth exercise

// TODO: Print error messages when arg too long
// TODO Check file parameters before performing operations on them.

int main(int argc, char const *argv[])
{   
    // Operating on command-line arguments:
    if (argc == 2) {
        char const *filename = argv[1];
        FILE* inputFilePointer = fopen(filename, "rb");
        if (inputFilePointer == NULL) {
            printf("Error while opening a file.");
            return 1;
        }
        BITMAPFILEHEADER fileHeader;
        BITMAPINFOHEADER fileInfoHeader;
        fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
        fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);
        print_header(filename);
        printf("\n");
        
        if (fileInfoHeader.biCompression == 0 && fileInfoHeader.biBitCount == 24) {
            Pixel** pixels = putIntoPixels(filename);
            printHistogram(filename, pixels);
        }
        else {
            printf("Histogram calculation unsupported for this kind of image.");
        }
        fclose(inputFilePointer);

        printf("\nDecode steganography? [y/n]");
        char answer;
        scanf(" %c", &answer);
        if (answer == 'y') {
            decypher(filename);
        }
        else if (answer == 'n') {
            printf("Steganography is not going to be decoded.");
        }
        else {
            printf("Invalid input.");
        }
    }

    else if (argc == 3) {
        char const *inputFilename = argv[1];
        char const *outputFilename = argv[2];

        FILE* inputFilePointer = fopen(inputFilename, "rb");
        if (inputFilePointer == NULL) {
            printf("Error while opening a file.");
            return 1;
        }
        BITMAPFILEHEADER fileHeader;
        BITMAPINFOHEADER fileInfoHeader;
        fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
        fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

        if (fileInfoHeader.biCompression == 0 && fileInfoHeader.biBitCount == 24) {
            Pixel** pixels = putIntoPixels(inputFilename);
            intoGreyscale(inputFilename, outputFilename, pixels);
        }
        fclose(inputFilePointer);
    }

    else if (argc == 4) {
        char const *inputFilename = argv[1];
        char const *outputFilename = argv[2];
        char const *message = argv[3];
        Pixel **pixels = putIntoPixels(inputFilename);

        copyFile(inputFilename, outputFilename, pixels);
        steganography(message, outputFilename);
    }
    return 0;
}