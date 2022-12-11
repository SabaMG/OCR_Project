#pragma once

Uint32 get_pixel_sobel(Uint32* pixels, int i, int j, int w);
void put_pixel(Uint32* pixels, int i, int j, int w, Uint32 pixel);
SDL_Surface *sobel(SDL_Surface *image);
