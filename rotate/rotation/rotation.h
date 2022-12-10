#pragma once

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
SDL_Surface* RotationCentral(SDL_Surface *origine, double angle);
