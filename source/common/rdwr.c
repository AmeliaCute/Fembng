#include "rdwr.h"

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

/**
 * Writes RGB pixel data to a binary file in run-length encoded format.
 *
 * @param filename Path to output binary file
 * @param width Width of image
 * @param height Height of image
 * @param pixels RGB pixel data to encode and write
 * @return 0 on success, non-zero on failure
 */
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
    applyQuantization(width, height, pixels, 64);
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

/**
 * Reads a JPEG image file into RGB pixel data.
 *
 * @param filename Path to JPEG file
 * @param width Pointer to int to store image width
 * @param height Pointer to int to store image height
 * @param pixels Pointer to RGB pixel array to fill with image data
 * @return 0 on success, non-zero on failure
 */
int readJPEGFile(const char *filename, int *width, int *height, RGB **pixels)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile;
    JSAMPARRAY buffer;
    int row_stride;

    if ((infile = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    *pixels = (RGB *)malloc((*width) * (*height) * sizeof(RGB));

    while (cinfo.output_scanline < cinfo.output_height)
    {
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int i = 0; i < cinfo.output_width; ++i)
        {
            (*pixels)[(cinfo.output_scanline - 1) * (*width) + i].r = buffer[0][i * cinfo.output_components];
            (*pixels)[(cinfo.output_scanline - 1) * (*width) + i].g = buffer[0][i * cinfo.output_components + 1];
            (*pixels)[(cinfo.output_scanline - 1) * (*width) + i].b = buffer[0][i * cinfo.output_components + 2];
        }
    }

    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
    return 0;
}
