#pragma once

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void inline set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
SDL_Surface *SDL_RotationCentral(SDL_Surface *origine, float angle);
