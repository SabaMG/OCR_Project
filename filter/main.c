#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "sobel.h"

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
	if (argc != 3)
		errx(EXIT_FAILURE, "Usage: imageIN imageOUT");

	SDL_Surface* surface = Sobel(load_image(argv[1]));

	IMG_SaveJPG(surface, argv[2], 100);
	SDL_FreeSurface(surface);

	return EXIT_SUCCESS;
}
