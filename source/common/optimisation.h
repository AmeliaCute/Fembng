#ifndef HEADER_OPTIMISATION
#define HEADER_OPTIMISATION

#include "datastruct.h"
#include <stdio.h>

void applyQuantization(int width, int height, RGB *pixels, int numColors);
void chromaSubsampling(int width, int height, RGB *pixels);

#endif //! HEADER_OPTIMISATION