#ifndef HEADER_CONVERTER
#define HEADER_CONVERTER

#include "datastruct.h"
#include "rdwr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <string.h>

int convertJPEGToFemboy(const char *inputFile, const char *outputFile);

#endif //! HEADER_CONVERTER