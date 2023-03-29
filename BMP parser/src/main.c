#include <utils.h>

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

int main(int argc, char const *argv[])
{
    // open the file
    FILE* inputFilePointer = fopen("tux.bmp", "rb");
    if (inputFilePointer == NULL)
    {
        printf("Error while opening a file.");
        return 1;
    }

    // Read the header data
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    unsigned char *fileFullHeader = malloc(fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader));
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFilePointer);
    fread(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, inputFilePointer);
    fread(fileFullHeader, fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(fileInfoHeader), 1, inputFilePointer);

    Pixel **pixels = putIntoPixels(inputFilePointer, fileHeader, fileInfoHeader);

    FILE* outputFilePointer = fopen("greyscaletux.bmp", "wb");
    if (outputFilePointer == NULL) {
        printf("Error while opening output file.");
        return 1;
    }
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

    // printHistogram(pixels, fileInfoHeader.biWidth, fileInfoHeader.biHeight);
    print_header(fileHeader, fileInfoHeader);

    fclose(inputFilePointer);
    for (int i = 0; i < fileInfoHeader.biHeight; i++) {
        free(pixels[i]); 
    }
    free(pixels);
    return 0;
}