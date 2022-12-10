#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>

// import sdl library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "gamma.h"

void help()
{
    char* str = "./main [image.jpg] [res.jpg]";
    errx(1, str);
}

int main(int argc, char** argv)
{
    if(argc != 3)
        help();

    // Init sdl window and image
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load(argv[1]);
    SDL_Surface* image = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);

    filter_image(image, 1.2);

    // Save grid result
    IMG_SaveJPG(image, argv[2], 100);
    SDL_FreeSurface(image);

    return EXIT_SUCCESS;
}
