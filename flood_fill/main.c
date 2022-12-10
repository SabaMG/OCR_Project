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
    char* str = "./main [image.jpg] [sobel.jpg] [res.jpg]";
    errx(1, str);
}

int main(int argc, char** argv)
{
    if(argc != 4)
        help();

    // Init sdl window and image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load(argv[1]);
    SDL_Surface* grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);
    init_grid = IMG_Load(argv[2]);
    SDL_Surface* sobel = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);
    
    SDL_Surface* croped_grid = crop_grid(grid, sobel);

    // Save grid result
    IMG_SaveJPG(croped_grid, argv[3], 100);
    SDL_FreeSurface(croped_grid);
    SDL_FreeSurface(grid);
    SDL_FreeSurface(sobel);

    return EXIT_SUCCESS;
}
