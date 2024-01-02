#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/rdwr.h"
#include "../common/datastruct.h"
#include "../common/convert.h"


/**
 * Generates a rainbow color gradient across the given pixel buffer.
 *
 * @param width Width of the pixel buffer.
 * @param height Height of the pixel buffer.
 * @param pixels Pointer to the pixel buffer.
 * 
 * For each pixel, the red component is set based on the y position,
 * the green component on the inverse of y, and the blue component
 * on the x position. Each color component ranges from 0 to 255.
 *
 * This results in a smooth rainbow gradient across the entire buffer.
 */
void generateRainbowGradient(int width, int height, RGB *pixels)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            double normalizedX = (double)j / (double)(width - 1);
            double normalizedY = (double)i / (double)(height - 1);

            pixels[i * width + j].r = (unsigned char)(normalizedY * 255.0);
            pixels[i * width + j].g = (unsigned char)((1.0 - normalizedY) * 255.0);
            pixels[i * width + j].b = (unsigned char)(normalizedX * 255.0);
        }
    }
}

/**
 * Sets all pixels in a 2D pixel buffer to a given color.
 *
 * @param width Width of the pixel buffer.
 * @param height Height of the pixel buffer.
 * @param pixels Pointer to the pixel buffer.
 * @param color The color to set all pixels to.
 */
void setAllPixelsToColor(int width, int height, RGB *pixels, RGB color)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            pixels[i * width + j] = color;
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s -w <width> -h <height> -o <output_file> [-q number_colors] [-c <input_file>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int width = 8, height = 8;
    unsigned char *quantization;
    char *outputFileName = "test";
    char *inputFileName = NULL;

    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-w") == 0)
        {
            width = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            height = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            outputFileName = argv[i + 1];
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            inputFileName = argv[i + 1];
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            quantization = argv[i + 1];
        }  // TODO: Implement quantization option
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    if (inputFileName) {
        if (convertJPEGToFemboy(inputFileName, outputFileName) != 0)
        {
            fprintf(stderr, "Failed to convert JPEG to femboy\n");
            return EXIT_FAILURE;
        }
    } else {
        // Generate rainbow gradient
        RGB *pixels = (RGB *)malloc(width * height * sizeof(RGB));
        if (!pixels)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return EXIT_FAILURE;
        }

        generateRainbowGradient(width, height, pixels);

        char outputFileNameWithExt[256];
        strcpy(outputFileNameWithExt, outputFileName);
        strcat(outputFileNameWithExt, ".femboy");

        if (writeBinaryFile(outputFileNameWithExt, width, height, pixels) != 0)
        {
            fprintf(stderr, "Failed to write output file\n");
            free(pixels);
            return EXIT_FAILURE;
        }

        free(pixels);
    }

    return EXIT_SUCCESS;
}
