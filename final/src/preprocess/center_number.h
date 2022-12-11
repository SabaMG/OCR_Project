
#pragma once

#include <stddef.h>
#include <SDL2/SDL.h>

void ajuste_case_fill(SDL_Surface* sobel, SDL_Rect* case_);
int average_pixel(Uint32 pixel, SDL_PixelFormat* format);
int average_surface(SDL_Surface* s);
int center_pixel(SDL_Surface* s, int* x, int* y, float seuil);
void ajuste_case(SDL_Surface* source, SDL_Rect* case_);
void remove_lines(SDL_Rect* case_, int degre);
void undo_remove_lines(SDL_Rect* case_, int degre);
SDL_Surface* resize(SDL_Surface *source, float zoom);
