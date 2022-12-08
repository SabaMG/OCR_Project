#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>

// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import files
#include "flood_fill.h"

void help()
{
    char* str = "./main [binarised_image.jpg] [res_image.jpg]";
    errx(1, str);
}

int main(int argc, char** argv)
{
    if(argc != 2)
        help();

    // Init sdl window and image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load(argv[1]);
    SDL_Surface* grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);
    
    Uint32* pixels = grid->pixels;
    printf("w = %i, h = %i\n", grid->w, grid->h/2);
    int m = (grid->h/2) * grid->w + 50;
    int pixel_w = pos(grid, m, grid->w);
    printf("pixels : %i, color = %X\n", pixel_w, pixels[pixel_w]);
    test(grid, pixel_w);

    pixels[pixel_w] = 0xFF00FF00;

    // Save grid result
    IMG_SaveJPG(grid, "res.jpg", 100);
    SDL_FreeSurface(grid);

    return EXIT_SUCCESS;
}
