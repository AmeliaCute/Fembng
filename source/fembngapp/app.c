//COMPLETE GARBAGE NEED REWRITE

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../common/rdwr.h"
#include "../common/datastruct.h"
#include "../common/bicubic.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MOVE_SPEED 7

void renderImage(SDL_Renderer *renderer, SDL_Texture *texture, int width, int height, float zoom, int offsetX, int offsetY)
{
    SDL_RenderClear(renderer);

    int destWidth = (int)(width * zoom);
    int destHeight = (int)(height * zoom);

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

    bicubicFilter(width, height, pixels);
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(RGB));

    printf("Initialized SDL, window, renderer, and texture\n");

    int quit = 0;
    float zoom = 1.0;
    int offsetX = 0;
    int offsetY = 0;
    int isDragging = 0;
    int updateCounter = 0;

    renderImage(renderer, texture, width, height, zoom, offsetX, offsetY);
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
                renderImage(renderer, texture, width, height, zoom, offsetX, offsetY);
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
                    renderImage(renderer, texture, width, height, zoom, offsetX, offsetY);
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
