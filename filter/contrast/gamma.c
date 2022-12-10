#include <stdio.h>
#include <math.h>

// import sdl library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "gamma.h"

Uint8 func_gamma(Uint8 pixel, float gamma)
{
    return (Uint8)(pow((((float)pixel)/255), (1 / gamma)) * 255);
}

void gamma_image(SDL_Surface* image, float gamma)
{
    Uint32* pixels = image->pixels;
    int size = image->w * image->h;

    for(int i = 0; i < size; ++i)
    {
	// collect rgb value of pixel
	Uint8 r, g ,b;
	SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
	
	// apply gamma filter on pixel values
	r = func_gamma(r, gamma);
	g = func_gamma(g, gamma);
	b = func_gamma(b, gamma);

	// modif pixel valu
	pixels[i] = SDL_MapRGB(image->format, r, g, b);
    }
}
