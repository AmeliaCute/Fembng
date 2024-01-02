#include "optimisation.h"

/**
 * Applies uniform quantization to the given RGB pixel data.
 * Divides each color channel (R, G, B) by the specified number of colors,
 * rounds down to the nearest integer, then multiplies back.
 * This reduces the number of distinct colors in the image.
 */
void applyQuantization(int width, int height, RGB *pixels, int numColors)
{
    int factor = 256 / numColors;

    for (int i = 0; i < width * height; ++i)
    {
        pixels[i].r = (pixels[i].r / factor) * factor;
        pixels[i].g = (pixels[i].g / factor) * factor;
        pixels[i].b = (pixels[i].b / factor) * factor;
    }
}

/**
 * Performs chroma subsampling on the given RGB pixel data by averaging 2x2 blocks.
 * Reduces the horizontal and vertical resolution by half.
 */
void chromaSubsampling(int width, int height, RGB *pixels)
{
    printf("Chroma subsampling...\n");
    for (int i = 0; i < height; i += 2)
    {
        for (int j = 0; j < width; j += 2)
        {
            RGB avgColor;
            avgColor.r = (pixels[i * width + j].r + pixels[i * width + j + 1].r +
                          pixels[(i + 1) * width + j].r + pixels[(i + 1) * width + j + 1].r) /
                         4;

            avgColor.g = (pixels[i * width + j].g + pixels[i * width + j + 1].g +
                          pixels[(i + 1) * width + j].g + pixels[(i + 1) * width + j + 1].g) /
                         4;

            avgColor.b = (pixels[i * width + j].b + pixels[i * width + j + 1].b +
                          pixels[(i + 1) * width + j].b + pixels[(i + 1) * width + j + 1].b) /
                         4;

            pixels[i * width + j] = avgColor;
            pixels[i * width + j + 1] = avgColor;
            pixels[(i + 1) * width + j] = avgColor;
            pixels[(i + 1) * width + j + 1] = avgColor;
        }
    }
    printf("Done\n");
}