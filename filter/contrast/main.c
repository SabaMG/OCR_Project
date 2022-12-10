#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

// import sdl library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "contrast.h"
#include "gamma.h"
#include "gauss.h"

void help()
{
    char* str = "./main [image.jpg] [res.jpg] [filtre] [val]";
    errx(1, str);
}

int main(int argc, char** argv)
{
    if(argc < 4)
        help();

    // Init sdl window and image
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load(argv[1]);
    SDL_Surface* image = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);

    if (strcmp(argv[3], "-gauss") == 0)
    {
	image = blur(image, 3);
    }
    else if(strcmp(argv[3], "-cont") == 0)
    {
	contrast_image(image);
    }
    else if(strcmp(argv[3], "-gamma") ==0)
    {
	if(argc != 5)
	    help();

	float val = atof(argv[4]);
	gamma_image(image, val);
    }

    // Save grid result
    IMG_SaveJPG(image, argv[2], 100);
    SDL_FreeSurface(image);

    return EXIT_SUCCESS;
}
