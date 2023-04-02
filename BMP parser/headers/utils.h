#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

// https://docs.microsoft.com/pl-pl/previous-versions/dd183376(v=vs.85)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct Pixel {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} Pixel;
#pragma pack(pop)

Pixel** putIntoPixels(const char* filename) {
    FILE* inputFilePointer = fopen(filename, "rb");
    if (inputFilePointer == NULL) {
      printf("Error while opening a file.");
      return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    int width = fileInfoHeader.biWidth;
    int height = fileInfoHeader.biHeight;
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;

    Pixel **pixels = malloc(height * sizeof(Pixel *));
    for (int i = 0; i < height; i++) {
        pixels[i] = (Pixel *)malloc(width * sizeof(Pixel));
    }

    fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET); // from the beginning to bfOffBits

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Pixel pixel;
            fread(&pixel.blue, sizeof(uint8_t), 1, inputFilePointer);
            fread(&pixel.green, sizeof(uint8_t), 1, inputFilePointer);
            fread(&pixel.red, sizeof(uint8_t), 1, inputFilePointer);
            pixels[i][j] = pixel;
        }
        fseek(inputFilePointer, padding, SEEK_CUR); // Move the file pointer to the beginning of the next row of pixels
    }

    fclose(inputFilePointer);
    return pixels;
}

void intoGreyscale(const char* inputFilename, const char* outputFilename, Pixel** pixels) {
    FILE* inputFilePointer = fopen(inputFilename, "rb");
    if (inputFilePointer == NULL) {
      printf("Error while opening the file.");
      return;
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    unsigned char *fileFullHeader = malloc(fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader));
    fread(fileFullHeader, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), 1, inputFilePointer);
    FILE* outputFilePointer = fopen(outputFilename, "wb");    

    fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFilePointer);
    fwrite(&fileInfoHeader, 1, sizeof(BITMAPINFOHEADER), outputFilePointer);
    fwrite(fileFullHeader, 1, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), outputFilePointer);
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;
    const unsigned char pixelPadding = '0';
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
            fwrite(&pixelPadding, 1, 1, outputFilePointer);
        }
    }
    fclose(inputFilePointer);
    fclose(outputFilePointer);
}

void copyFile(const char* inputFilename, const char* outputFilename, Pixel** pixels) {
    FILE* inputFilePointer = fopen(inputFilename, "rb");
    if (inputFilePointer == NULL) {
        printf("Error while opening the file.");
        return;
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    unsigned char *fileFullHeader = malloc(fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader));
    fread(fileFullHeader, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), 1, inputFilePointer);
    FILE* outputFilePointer = fopen(outputFilename, "wb");
    fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFilePointer);
    fwrite(&fileInfoHeader, 1, sizeof(BITMAPINFOHEADER), outputFilePointer);
    fwrite(fileFullHeader, 1, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), outputFilePointer);
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;
    const unsigned char pixelPadding = '0';

    for (int i = 0; i < fileInfoHeader.biHeight; i++) {
        for (int j = 0; j < fileInfoHeader.biWidth; j++) {
            unsigned char pixelValues[3] = {pixels[i][j].blue, pixels[i][j].green, pixels[i][j].red};
            fwrite(pixelValues, 3, 1, outputFilePointer);
        }
        for (int k = 0; k < padding; k++) {
            fwrite(&pixelPadding, 1, 1, outputFilePointer);
        }
    }
    fclose(inputFilePointer);
    fclose(outputFilePointer);
}

void printHistogram(const char* filename, Pixel** pixels) {
    FILE* inputFilePointer = fopen(filename, "rb");
    if (inputFilePointer == NULL) {
        printf("Error while opening the file.");
        return;
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);
    
    int width = fileInfoHeader.biWidth;
    int height = fileInfoHeader.biHeight;
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;
    const unsigned char pixelPadding = '0';

    int red_count[16] = {0};
    int green_count[16] = {0};
    int blue_count[16] = {0};

    int numberOfPixels = 0;
    // printf()

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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

#endif