#ifndef HEADER_OPTIMISATION
#define HEADER_OPTIMISATION

#include "datastruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <complex.h>

void applyQuantization(int width, int height, RGB *pixels, int numColors);
void chromaSubsampling(int width, int height, RGB *pixels);
void applyLowPassFilter(int width, int height, RGB *pixels);
void applyLaplaceFilter(int width, int height, RGB *pixels);


#endif //! HEADER_OPTIMISATION