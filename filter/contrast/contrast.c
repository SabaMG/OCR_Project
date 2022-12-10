#include <stdio.h>
#include <math.h>

// import sdl library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "contrast.h"

void range_intensity(SDL_Surface* image, Uint8 min[], Uint8 max[])
{
    Uint32* pixels = image->pixels;
    int size = image->w * image->h;

    //min   r    g    b
    min[0] = 255;
    min[1] = 255;
    min[2] = 255;
    //max  r  g  b
    max[0] = 0;
    max[1] = 0;
    max[2] = 0;

    for(int i = 0; i < size; ++i)
    {
	// collect rgb value of pixel
	Uint8 r, g ,b;
	SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
	Uint8 color[] = {r, g, b};
	
	// calculate intensity of pixel
	for(int i = 0; i < 3; ++i)
	{
	    if(color[i] < min[i])
		min[i] = color[i];
	    else if (color[i] > max[i])
		max[i] = color[i];
	}
    }
}

void contrast_image(SDL_Surface* image)
{
    Uint32* pixels = image->pixels;
    int size = image->w * image->h;

    // collect color bounds
    Uint8 min_pixel[3];
    Uint8 max_pixel[3];
    range_intensity(image, min_pixel, max_pixel);
    Uint8 min = 0;
    Uint8 max = 255;

    for(int i = 0; i < size; ++i)
    {
	// collect rgb value of pixel
	Uint8 r, g ,b;
	SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
	Uint8 color[] = {r, g, b};
	
	// nomalize each color
	for(int i = 0; i < 3; ++i)
	{
	    color[i] =
		(Uint8)((color[i] - min_pixel[i]) * ((float)(max - min) / (float)(max_pixel[i] - min_pixel[i])));
	}

	// modif pixel valu
	pixels[i] = SDL_MapRGB(image->format, color[0], color[1], color[2]);
    }
}
