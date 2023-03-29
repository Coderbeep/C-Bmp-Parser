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

Pixel** putIntoPixels(FILE* filePointer, BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER fileInfoHeader) {
    int width = fileInfoHeader.biWidth;
    int height = fileInfoHeader.biHeight;
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3;

    Pixel **pixels = malloc(height * sizeof(Pixel *));
    for (int i = 0; i < height; i++) {
        pixels[i] = (Pixel *)malloc(width * sizeof(Pixel));
    }

    fseek(filePointer, fileHeader.bfOffBits, SEEK_SET); // from the beginning to bfOffBits

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Pixel pixel;
            fread(&pixel.blue, sizeof(uint8_t), 1, filePointer);
            fread(&pixel.green, sizeof(uint8_t), 1, filePointer);
            fread(&pixel.red, sizeof(uint8_t), 1, filePointer);
            pixels[i][j] = pixel;
        }
        fseek(filePointer, padding, SEEK_CUR); // Move the file pointer to the beginning of the next row of pixels
    }

    return pixels;
}

#endif