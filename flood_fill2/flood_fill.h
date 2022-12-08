#pragma once

void put_color(Uint32* pixels, char* M, int size, Uint32 color);
int pos(SDL_Surface* grid, int middle_h, int w);
void test(SDL_Surface* grid, int pos);
void f_fill(Uint32* pixels, int begin_pixel, char* M, int w, int size, Uint32 color);
int next_to(Uint32* pixels, int curr, Uint32 color, int w, int size, int next[4]);
