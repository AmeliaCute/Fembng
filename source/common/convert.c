#include "convert.h"

int convertJPEGToFemboy(const char *inputFile, const char *outputFile )
{
    RGB *jpegPixels;
    int jpegWidth, jpegHeight;

    // Read JPEG file
    if (readJPEGFile(inputFile, &jpegWidth, &jpegHeight, &jpegPixels) != 0)
    {
        fprintf(stderr, "Failed to read JPEG file\n");
        return EXIT_FAILURE;
    }

    char outputFileWithExt[256];
    strcpy(outputFileWithExt, outputFile);
    strcat(outputFileWithExt, ".femboy");

    if (writeBinaryFile(outputFileWithExt, jpegWidth, jpegHeight, jpegPixels) != 0)
    {
        fprintf(stderr, "Failed to write output file\n");
        free(jpegPixels);
        return EXIT_FAILURE;
    }

    free(jpegPixels);

    return EXIT_SUCCESS;
}