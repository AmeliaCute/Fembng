#include "convert.h"

/**
 * Converts a JPEG image to a Fembng binary image file.
 *
 * Reads the JPEG image from the input file, converts it to the Fembng
 * binary format, and writes the output to a file with .femboy extension.
 *
 * Returns EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int convertJPEGToFemboy(const char *inputFile, const char *outputFile)
{
    RGB *jpegPixels;
    int jpegWidth, jpegHeight;

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