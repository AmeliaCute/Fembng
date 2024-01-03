#ifndef HEADER_BICUBIC
#define HEADER_BICUBIC

#include "datastruct.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void computeCoefficients(double x, double coeffs[7]);
double polyInterpolate(double p[7], double coeffs[7]);
void bicubicFilter(int width, int height, RGB *pixels);

#endif //! HEADER_BICUBIC