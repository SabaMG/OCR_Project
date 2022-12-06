#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>

// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// import files
#include "draw_result.h"
#include "file.h"

void help()
{
    char* str = "./main [grid] [grid.result] [empty_grid.jpg] [res.jpg]";
    errx(1, str);
}

int main(int argc, char** argv)
{
    // Init sdl window and image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load(argv[3]);
    SDL_Surface* empty_grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);

    // save center coordinate of each boxes
    int coor[81][2] = {};
    collect_boxes_coor(empty_grid->w, empty_grid->h, coor, 9);
    
    // Read grid and result grid
    int grid[SIZE][SIZE];
    int result[SIZE][SIZE];
    read_grid(grid, argv[1]);
    read_grid(result, argv[2]);

    // make empty grid
    SDL_Surface* res = draw_image(empty_grid, grid, result, coor);

    // Save grid result
    IMG_SaveJPG(res, "res.jpg", 100);
    SDL_FreeSurface(res);

    return EXIT_SUCCESS;
}
