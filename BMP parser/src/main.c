#include <utils.h>
#include <stdlib.h>
#include <string.h>

// Exercise 1
void print_header(BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER fileInfoHeader) {
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
}

// Exercise 2
void printHistogram(Pixel **pixels, int width, int height) {
    int red_count[16] = {0};
    int green_count[16] = {0};
    int blue_count[16] = {0};

    int numberOfPixels = 0;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            red_count[pixels[i][j].red / 16]++;
            green_count[pixels[i][j].green / 16]++;
            blue_count[pixels[i][j].blue / 16]++;

            numberOfPixels++;
        }
    }

    printf("Blue:\n");
    for (int i = 0; i < 16; i++) {
        printf("  %d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (float)blue_count[i]/ (float) numberOfPixels * 100);
    }
    printf("Green:\n");
    for (int i = 0; i < 16; i++) {
        printf("  %d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (float)green_count[i]/ (float) numberOfPixels * 100);
    }
    printf("Red:\n");
    for (int i = 0; i < 16; i++) {
        printf("  %d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (float)red_count[i]/ (float) numberOfPixels * 100);
    }
}

void intoGreyscale(FILE* outputFilePointer, BITMAPFILEHEADER fileHeader, 
                   BITMAPINFOHEADER fileInfoHeader, unsigned char* fileFullHeader, Pixel **pixels) {

    fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFilePointer);
    fwrite(&fileInfoHeader, 1, sizeof(BITMAPINFOHEADER), outputFilePointer);
    fwrite(fileFullHeader, 1, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), outputFilePointer);
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;
    unsigned char pixelPadding[3] = {0, 0, 0};
    for (int i = 0; i < fileInfoHeader.biHeight; i++) {
        for (int j = 0; j < fileInfoHeader.biWidth; j++) {
            uint8_t blue = pixels[i][j].blue;
            uint8_t green = pixels[i][j].green;
            uint8_t red = pixels[i][j].red;
            int meanValue = (blue + green + red) / 3;
            unsigned char meanPixelValues[3] = {meanValue, meanValue, meanValue};
            fwrite(meanPixelValues, 3, 1, outputFilePointer);
        }
        for (int k = 0; k < padding; k++) {
            fwrite(pixelPadding, 3, 1, outputFilePointer);
        }
    }
}

// TODO: open files separately in every exercise
// First open a file, read the header, info header and the rest
// and close the file

// TODO: First 8 bytes of encoding are used for the number
// describing how many characters you want to encode
// print alerts if more

void putToBinary(char letter, char *dest) {
    for (int i = 7; i >= 0; i--) {
        int bit = (letter >> i) & 1;
        dest[7-i] = bit + '0';
    }
    dest[8] = '\0';
}

void steganography(char *textToEncode) {
    FILE* inputFilePointer = fopen("tux.bmp", "rb+");
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
    printf("Padding after a line (bytes): %d\nNumber of letters that can fit in a line: %d\n", padding, numLettersLine);

    char letters[strlen(textToEncode) + 1][9];
    putToBinary(strlen(textToEncode), letters[0]); 
    // Put the number as a first thing to encode
    // the number represents the length of a sentence to encode

    for (int i = 1; i < strlen(textToEncode) + 1; i++) {
        putToBinary(textToEncode[i - 1], letters[i]);
    }

    for (int i = 0; i < strlen(textToEncode) + 1; i++) {
        printf("%s\n", letters[i]);
    }
    // Storing binary representation of chars in the sentence: DONE
    // char *output = letters[0];
    // char c = strtol(output, 0, 2);
    // printf("%c\n", c);

    // for (int i = 0; i < strlen(textToEncode) + 1; )
    // unsigned char pixelsLetter[8];
    // fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET);
    // fread(pixelsLetter, 8, 1, inputFilePointer);
    // for (int  i = 0; i < 8; i ++) {
    //     printf("%d ", pixelsLetter[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 8; i++) {
    //     int pixelVal = (int) pixelsLetter[i];
    //     if (output[i] == '0'){
    //         if (pixelVal % 2 == 1) {
    //             pixelVal -= 1;
    //         }
    //     }
    //     else {
    //         if (pixelVal % 2 == 0) {
    //             pixelVal += 1;
    //         }
    //     }
    //     pixelsLetter[i] = pixelVal;
    // };
    
    // for (int  i = 0; i < 8; i ++) {
    //     printf("%d ", pixelsLetter[i]);
    // }
    // fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET);
    // fwrite(pixelsLetter, 8, 1, inputFilePointer);
    fclose(inputFilePointer);
}


int main(int argc, char const *argv[])
{
    char *textToEncode = "Help!";
    steganography(textToEncode);
    return 0;
}