#include "optimisation.h"

/**
 * Applies quantization to the given RGB pixel data.
 * Separates the DC and AC components.
 */
void applyQuantization(int width, int height, RGB *pixels, int numColors)
{
    int factor = 256 / numColors;

    for (int i = 0; i < height; i += 8) {
        for (int j = 0; j < width; j += 8) {
            pixels[i * width + j].r = (pixels[i * width + j].r / factor) * factor;
            pixels[i * width + j].g = (pixels[i * width + j].g / factor) * factor;
            pixels[i * width + j].b = (pixels[i * width + j].b / factor) * factor;

            for (int k = i; k < i + 8; ++k) {
                for (int l = j + 1; l < j + 8; ++l) {
                    pixels[k * width + l].r = (pixels[k * width + l].r / factor) * factor;
                    pixels[k * width + l].g = (pixels[k * width + l].g / factor) * factor;
                    pixels[k * width + l].b = (pixels[k * width + l].b / factor) * factor;
                }
            }
        }
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

/**
 * Applies a 3x3 low pass filter kernel to the given RGB pixel data.
 * Uses a normalized box filter kernel to blur the image.
 * Operates on all channels separately.
 */
void applyLowPassFilter(int width, int height, RGB *pixels)
{
    printf("Applying Low-Pass Filter...\n");

    double lowPassKernel[3][3] = {
        {1.0 / 9, 1.0 / 9, 1.0 / 9},
        {1.0 / 9, 1.0 / 9, 1.0 / 9},
        {1.0 / 9, 1.0 / 9, 1.0 / 9}};

    RGB *tempPixels = (RGB *)malloc(width * height * sizeof(RGB));
    if (!tempPixels)
    {
        perror("Bad alloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                double sum = 0.0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].r * lowPassKernel[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].r = (unsigned char)fmin(fmax(sum, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                double sum = 0.0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].g * lowPassKernel[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].g = (unsigned char)fmin(fmax(sum, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                double sum = 0.0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].b * lowPassKernel[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].b = (unsigned char)fmin(fmax(sum, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                pixels[i * width + j + k] = tempPixels[i * width + j + k];
            }
        }
    }

    free(tempPixels);
    printf("Done\n");
}

/**
 * Applies a Laplace filter to sharpen the given image pixels.
 *
 * The filter coefficients are defined in a 3x3 kernel. For each pixel,
 * the kernel is overlapped with its 8 surrounding pixels. The weighted sum
 * of these values is used to sharpen the R, G, and B channels independently.
 *
 * Pixels along the border are not filtered to avoid going out of bounds.
 * A temporary buffer holds the sharpened results before copying back to
 * the input pixels array.
 */
void applyLaplaceFilter(int width, int height, RGB *pixels)
{
    printf("LaPlace Filter.. (sharpening)...\n");

    int laplaceFilter[3][3] = {
        {0, -1, 0},
        {-1, 4, -1},
        {0, -1, 0}};

    RGB *tempPixels = (RGB *)malloc(width * height * sizeof(RGB));
    if (!tempPixels)
    {
        perror("Allocation mémoire échouée");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                int sum = 0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].r * laplaceFilter[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].r = (unsigned char)fmin(fmax(pixels[i * width + j + k].r + sum, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                int sum = 0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].g * laplaceFilter[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].g = (unsigned char)fmin(fmax(pixels[i * width + j + k].g + sum, 0.0), 255.0);

                sum = 0;

                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        sum += pixels[(i + m) * width + j + n].b * laplaceFilter[m + 1][n + 1];
                    }
                }

                tempPixels[i * width + j + k].b = (unsigned char)fmin(fmax(pixels[i * width + j + k].b + sum, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                pixels[i * width + j + k] = tempPixels[i * width + j + k];
            }
        }
    }

    free(tempPixels);
    printf("Done\n");
}