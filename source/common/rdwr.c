#include "rdwr.h"

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
 * Encodes RGB pixel data into run-length encoded format.
 *
 * @param pixels Pointer to RGB pixel data to encode
 * @param size Number of pixels in the pixel data array
 * @param encodedData Pointer to RLE array pointer to store encoded data
 * @param encodedSize Pointer to int to store number of RLE elements
 */
void runLengthEncode(RGB *pixels, int size, RLE **encodedData, int *encodedSize)
{
    printf("Run-length encoding...\n");
    *encodedData = (RLE *)malloc(size * sizeof(RLE));
    if (!(*encodedData))
    {
        perror("Bad alloc");
        exit(EXIT_FAILURE);
    }

    int count = 1;
    int index = 0;

    for (int i = 1; i < size; ++i)
    {
        if (pixels[i].r == pixels[i - 1].r && pixels[i].g == pixels[i - 1].g && pixels[i].b == pixels[i - 1].b)
        {
            count++;
        }
        else
        {
            (*encodedData)[index].value = (pixels[i - 1].r) | (pixels[i - 1].g << 8) | (pixels[i - 1].b << 16);
            (*encodedData)[index].count = count;
            index++;
            count = 1;
        }
    }

    (*encodedData)[index].value = (pixels[size - 1].r) | (pixels[size - 1].g << 8) | (pixels[size - 1].b << 16);
    (*encodedData)[index].count = count;
    index++;
    printf("Encoded size: %d\n", index);

    printf("Done\n");
    *encodedSize = index;
}

int writeBinaryFile(const char *filename, int width, int height, RGB *pixels)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Cannot open");
        exit(EXIT_FAILURE);
    }

    fwrite(&width, sizeof(int), 1, file);
    fwrite(&height, sizeof(int), 1, file);

    RLE *encodedData;
    int encodedSize;

    chromaSubsampling(width, height, pixels);
    runLengthEncode(pixels, width * height, &encodedData, &encodedSize);

    fwrite(&encodedSize, sizeof(int), 1, file);
    fwrite(encodedData, sizeof(RLE), encodedSize, file);

    free(encodedData);
    fclose(file);
    return 0;
}

/**
 * Reads an image file in binary format into RGB pixel data.
 *
 * @param filename Path to the binary image file
 * @param width Pointer to an int to store the image width
 * @param height Pointer to an int to store the image height
 * @param pixels Pointer to an RGB pixel array pointer to store the decoded pixel data
 * @return 0 on success, non-zero on failure
 */
int readBinaryFile(const char *filename, int *width, int *height, RGB **pixels)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Cannot open file");
        return EXIT_FAILURE;
    }

    fread(width, sizeof(int), 1, file);
    fread(height, sizeof(int), 1, file);

    int encodedSize;
    fread(&encodedSize, sizeof(int), 1, file);

    RLE *encodedData = (RLE *)malloc(encodedSize * sizeof(RLE));
    if (!encodedData)
    {
        perror("Bad alloc");
        fclose(file);
        return EXIT_FAILURE;
    }

    fread(encodedData, sizeof(RLE), encodedSize, file);

    *pixels = (RGB *)malloc((*width) * (*height) * sizeof(RGB));
    if (!(*pixels))
    {
        perror("Bad alloc");
        free(encodedData);
        fclose(file);
        return EXIT_FAILURE;
    }

    int index = 0;
    for (int i = 0; i < encodedSize; ++i)
    {
        for (int j = 0; j < encodedData[i].count; ++j)
        {
            (*pixels)[index].r = (encodedData[i].value) & 0xFF;
            (*pixels)[index].g = (encodedData[i].value >> 8) & 0xFF;
            (*pixels)[index].b = (encodedData[i].value >> 16) & 0xFF;
            index++;

            if (index >= (*width) * (*height))
            {
                break;
            }
        }
    }

    free(encodedData);
    fclose(file);
    return 0;
}
