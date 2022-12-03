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

void help()
{
    char* str = "./main [file.result]";
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
    SDL_Surface* init_grid = IMG_Load("grille_vierge.jpg");
    SDL_Surface* empty_grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);

    // save center coordinate of each boxes
    int coor[81][2] = {};
    collect_boxes_coor(empty_grid->w, empty_grid->h, coor, 9);
    
    // write 0 to coor 0, 0
    SDL_Color black = {0, 126, 0, 255};
    char number = '1';
    for(size_t i = 0; i < 81; ++i)
    {
	char str[] = {number, 0};
	SDL_Surface* zero = create_number(str, 55, black, "font.ttf");
        write_on_grid(empty_grid, zero, coor[i][0], coor[i][1]);
	number = ((number - 47) % 10) + 48;
    }

    // Save grid result
    IMG_SaveJPG(empty_grid, argv[1], 100);
    SDL_FreeSurface(empty_grid);

    return EXIT_SUCCESS;
}
