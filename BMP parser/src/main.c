#include <stdio.h>
#include <stdint.h>

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

int main(int argc, char const *argv[])
{
    FILE* inputFilePointer = fopen("c-programming.bmp", "rb");
    if (inputFilePointer == NULL)
    {
        printf("Blocked");
        return 0;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);
    print_header(fileHeader, fileInfoHeader);
    fclose(inputFilePointer);
    return 0;
}