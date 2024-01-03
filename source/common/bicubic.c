#include "bicubic.h"

void computeCoefficients(double x, double coeffs[7]) {
    double x2 = x * x;
    double x3 = x2 * x;

    coeffs[0] = -0.5 * x3 + x2 - 0.5 * x;
    coeffs[1] = 1.5 * x3 - 2.5 * x2 + 1.0;
    coeffs[2] = -1.5 * x3 + 2.0 * x2 + 0.5 * x;
    coeffs[3] = 0.5 * x3 - 0.5 * x2;
    coeffs[4] = 0.0;
    coeffs[5] = 0.0;
    coeffs[6] = 0.0;
}
double polyInterpolate(double p[7], double coeffs[7]) 
{
    return (coeffs[0] * p[0] + coeffs[1] * p[1] + coeffs[2] * p[2] + coeffs[3] * p[3] +
           coeffs[4] * p[4] + coeffs[5] * p[5] + coeffs[6] * p[6]);
}
void bicubicFilter(int width, int height, RGB *pixels) {
    printf("Applying bicubic filter...\n");

    RGB *tempPixels = (RGB *)malloc(width * height * sizeof(RGB));
    if (!tempPixels) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    double coeffs[7] = {0.025, 0, 0.1, 0.3, 0.1, 0, 0.025};

    for (int i = 1; i < height - 2; i++) {
        for (int j = 1; j < width - 2; j++) {
            for (int k = 0; k < 3; k++) {
                double p[7] = {
                    pixels[(i - 1) * width + j - 1 + k].r,
                    pixels[(i - 1) * width + j + k].r,
                    pixels[(i - 1) * width + j + 1 + k].r,
                    pixels[i * width + j + 2 + k].r,
                    pixels[(i - 1) * width + j + 2 + k].r,
                    pixels[i * width + j + 1 + k].r,
                    pixels[i * width + j + k].r
                };

                double x = .25;
                computeCoefficients(x, coeffs);


                double interpolatedValue = polyInterpolate(p, coeffs);
                tempPixels[i * width + j + k].r = (unsigned char)fmin(fmax(interpolatedValue, 0.0), 255.0);

                p[0] = pixels[(i - 1) * width + j - 1 + k].g;
                p[1] = pixels[(i - 1) * width + j + k].g;
                p[2] = pixels[(i - 1) * width + j + 1 + k].g;
                p[3] = pixels[i * width + j + 2 + k].g;
                p[4] = pixels[(i - 1) * width + j + 2 + k].g;
                p[5] = pixels[i * width + j + 1 + k].g;
                p[6] = pixels[i * width + j + k].g;

                interpolatedValue = polyInterpolate(p, coeffs);
                tempPixels[i * width + j + k].g = (unsigned char)fmin(fmax(interpolatedValue, 0.0), 255.0);

                p[0] = pixels[(i - 1) * width + j - 1 + k].b;
                p[1] = pixels[(i - 1) * width + j + k].b;
                p[2] = pixels[(i - 1) * width + j + 1 + k].b;
                p[3] = pixels[i * width + j + 2 + k].b;
                p[4] = pixels[(i - 1) * width + j + 2 + k].b;
                p[5] = pixels[i * width + j + 1 + k].b;
                p[6] = pixels[i * width + j + k].b;

                interpolatedValue = polyInterpolate(p, coeffs);
                tempPixels[i * width + j + k].b = (unsigned char)fmin(fmax(interpolatedValue, 0.0), 255.0);
            }
        }
    }

    for (int i = 1; i < height - 2; i++) {
        for (int j = 1; j < width - 2; j++) {
            for (int k = 0; k < 3; k++) {
                pixels[i * width + j + k] = tempPixels[i * width + j + k];
            }
        }
    }

    free(tempPixels);
    printf("Done\n");
}