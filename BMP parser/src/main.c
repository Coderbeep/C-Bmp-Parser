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

int main(int argc, char const *argv[])
{
    FILE* inputFilePointer = fopen("logoC.bmp", "rb");
    if (inputFilePointer == NULL)
    {
        printf("Blocked");
        return 0;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);

    int width = fileInfoHeader.biWidth;
    int height = fileInfoHeader.biHeight;
    int padding = (floor((fileInfoHeader.biBitCount * fileInfoHeader.biWidth + 31) / 32) * 4) - fileInfoHeader.biWidth * 3; // Padding is used to align the rows of pixels to multiples of 4 bytes

    // Allocate memory for the pixels 
    Pixel **pixels = malloc(height * sizeof(Pixel *));
    for (int i = 0; i < height; i++) {
        pixels[i] = (Pixel *)malloc(width * sizeof(Pixel));
    }

    fseek(inputFilePointer, fileHeader.bfOffBits, SEEK_SET); // Move the file pointer to the beginning of the bitmap data

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

    printHistogram(pixels, width, height);
    fclose(inputFilePointer);
    for (int i = 0; i < height; i++) {
        free(pixels[i]); 
    }
    free(pixels);
    return 0;
}