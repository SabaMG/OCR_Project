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

#define GRID_SIZE 534

void help()
{
    char* str = "./main [grid] [grid.result] [empty_grid.jpg]";
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
    SDL_Surface* init_grid = IMG_Load("grille_vierge.jpg");
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
    make_grid_empty(empty_grid, grid, result, coor, "font.ttf", 55);

    // Save grid result
    IMG_SaveJPG(empty_grid, argv[3], 100);
    SDL_FreeSurface(empty_grid);

    return EXIT_SUCCESS;
}
