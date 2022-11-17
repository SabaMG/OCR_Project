#pragma once

#include <stddef.h>
#include <SDL2/SDL.h>

int average_pixel(Uint32 pixel, SDL_PixelFormat* format);
int average_surface(SDL_Surface* s);
void center_pixel(SDL_Surface* s, int* x, int* y, float seuil);
void ajuste_case(SDL_Surface* source, SDL_Rect* case_);
void remove_lines(SDL_Rect* case_);
