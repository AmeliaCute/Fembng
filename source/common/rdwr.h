#ifndef READER_WRITER_HEADER
#define READER_WRITER_HEADER

#include <stdio.h>
#include <stdlib.h>
#include "datastruct.h"

void chromaSubsampling(int width, int height, RGB *pixels);
void runLengthEncode(RGB *pixels, int size, RLE **encodedData, int *encodedSize);
int writeBinaryFile(const char *filename, int width, int height, RGB *pixels);
int readBinaryFile(const char *filename, int *width, int *height, RGB **pixels);

#endif // !READER_WRITER_HEADER