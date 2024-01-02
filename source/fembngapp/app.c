#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../common/rdwr.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MOVE_SPEED 7

void applyGaussianBlur(int width, int height, RGB *pixels, double sigma)
{
    int kernelSize = 25;
    int halfSize = kernelSize / 2;
    
    double kernel[kernelSize][kernelSize];
    double totalWeight = 0.0;

    for (int y = -halfSize; y <= halfSize; ++y)
    {
        for (int x = -halfSize; x <= halfSize; ++x)
        {
            kernel[y + halfSize][x + halfSize] = exp(-(x * x + y * y) / (2 * sigma * sigma));
            totalWeight += kernel[y + halfSize][x + halfSize];
        }
    }

    for (int y = 0; y < kernelSize; ++y)
    {
        for (int x = 0; x < kernelSize; ++x)
        {
            kernel[y][x] /= totalWeight;
        }
    }

    RGB *tempPixels = malloc(width * height * sizeof(RGB));
    if (!tempPixels)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            double totalR = 0, totalG = 0, totalB = 0;

            for (int ky = -halfSize; ky <= halfSize; ++ky)
            {
                for (int kx = -halfSize; kx <= halfSize; ++kx)
                {
                    int nx = x + kx;
                    int ny = y + ky;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                    {
                        double kernelValue = kernel[ky + halfSize][kx + halfSize];
                        totalR += pixels[ny * width + nx].r * kernelValue;
                        totalG += pixels[ny * width + nx].g * kernelValue;
                        totalB += pixels[ny * width + nx].b * kernelValue;
                    }
                }
            }

            tempPixels[y * width + x].r = (unsigned char)totalR;
            tempPixels[y * width + x].g = (unsigned char)totalG;
            tempPixels[y * width + x].b = (unsigned char)totalB;
        }
    }

    memcpy(pixels, tempPixels, width * height * sizeof(RGB));

    free(tempPixels);
}

void renderImage(SDL_Renderer *renderer, SDL_Texture *texture, int origWidth, int origHeight, int targetWidth, int targetHeight, float zoom, int offsetX, int offsetY) 
{
    SDL_RenderClear(renderer);

    int destWidth = (int)(targetWidth * zoom);
    int destHeight = (int)(targetHeight * zoom);

    int destX = SCREEN_WIDTH / 2 - destWidth / 2 + offsetX;
    int destY = SCREEN_HEIGHT / 2 - destHeight / 2 + offsetY;

    SDL_Rect destRect = {destX, destY, destWidth, destHeight};

    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    printf("Opening file: %s\n", filename);

    int width, height;
    RGB *pixels;
    readBinaryFile(filename, &width, &height, &pixels);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        free(pixels);
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *texture;

    if (!window || !renderer)
    {
        fprintf(stderr, "Window or renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        free(pixels);
        return EXIT_FAILURE;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, width, height);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest);
    applyGaussianBlur(width, height, pixels, .65);
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(RGB));

    printf("Initialized SDL, window, renderer, and texture\n");

    int quit = 0;
    float zoom = 1.0;
    int offsetX = 0;
    int offsetY = 0;
    int isDragging = 0;
    int updateCounter = 0;

    renderImage(renderer, texture, width, height, width * 2, height * 2, zoom, offsetX, offsetY);
    while (!quit)
    {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
                if (e.wheel.y > 0)
                    zoom *= 1.2;
                else if (e.wheel.y < 0)
                    zoom /= 1.2;
                renderImage(renderer, texture, width, height, width * 2, height * 2, zoom, offsetX, offsetY);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                isDragging = 1;
            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
                isDragging = 0;
            else if (e.type == SDL_MOUSEMOTION && isDragging)
            {
                offsetX += e.motion.xrel;
                offsetY += e.motion.yrel;
                if (updateCounter % MOVE_SPEED == 0)
                    renderImage(renderer, texture, width, height, width * 2, height * 2, zoom, offsetX, offsetY);
                updateCounter++;
            }
        }
    }

    printf("Cleaning up\n");

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(pixels);

    return EXIT_SUCCESS;
}
