#pragma once

SDL_Surface* crop_grid (SDL_Surface* grid);
void extract_data(char* M, int w, int h, int* start_pix, int* width, int* height);
void put_color(Uint32* pixels, char* M, int size, Uint32 color);
int pos(SDL_Surface* grid, int middle_h, int w);
void collect_form(SDL_Surface* grid, int pos, int* start, int* width, int* height);
void f_fill(Uint32* pixels, int begin_pixel, char* M, int w, int size, Uint32 color);
int next_to(Uint32* pixels, int curr, Uint32 color, int w, int size, int next[4]);
