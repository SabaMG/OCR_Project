#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <SDL2/SDL2_rotozoom.h>
#include "manual_rotation.h"

SDL_Surface* load_image(const char* path)
{
	SDL_Surface* tempSurface = IMG_Load(path);
	if (tempSurface == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_Surface* surface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(tempSurface);
	return surface;
}


int main(int argc, char** argv)
{
    if(argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");
    
    SDL_Surface* image = load_image(argv[1]);
    printf("Insert your angle :");
    char a[4];

    if (scanf("%s", a) >0) {
    	long angle = str_to_uint(a)%360;
    	Rotation_Manuel_F(image, (double)angle);
    }

    else {
    	printf("You did not enter any number.\n");
    }
}
