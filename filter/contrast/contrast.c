#include <stdio.h>
#include <math.h>

// import sdl library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "contrast.h"

void range_intensity(SDL_Surface* image, Uint8* min, Uint8* max)
{
    Uint32* pixels = image->pixels;
    int size = image->w * image->h;

    *min = 255;
    *max = 0;

    for(int i = 0; i < size; ++i)
    {
	// collect rgb value of pixel
	Uint8 r, g ,b;
	SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
	
	// calculate intensity of pixel
	Uint8 intensity = (r + g + b)/3;

	if(intensity < *min)
	    *min = intensity;
	else if (intensity > * max)
	    *max = intensity;
    }
}

void contrast_image(SDL_Surface* image)
{
    Uint32* pixels = image->pixels;
    int size = image->w * image->h;

    Uint8 min, max;
    range_intensity(image, &min, &max);

    for(int i = 0; i < size; ++i)
    {
	// collect rgb value of pixel
	Uint8 r, g ,b;
	SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
	
	// calculate intensity of pixel
	Uint8 intensity = (r + g + b)/3;

	float new_intensity = (((float)(intensity - min)) / ((float)(max - min)));
	r = r * new_intensity;
	g = g * new_intensity;
	b = b * new_intensity;

	// modif pixel valu
	pixels[i] = SDL_MapRGB(image->format, r, g, b);
    }
}
