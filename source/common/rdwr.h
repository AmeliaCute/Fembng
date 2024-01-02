#ifndef READER_WRITER_HEADER
#define READER_WRITER_HEADER

#include "datastruct.h"
#include "convert.h"
#include "optimisation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void runLengthEncode(RGB *pixels, int size, RLE **encodedData, int *encodedSize);
int writeBinaryFile(const char *filename, int width, int height, RGB *pixels);
int readBinaryFile(const char *filename, int *width, int *height, RGB **pixels);
int readJPEGFile(const char *filename, int *width, int *height, RGB **pixels);

#endif // !READER_WRITER_HEADER