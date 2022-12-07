#pragma once

struct box{
    int pos;
    int start;
    int end;
    int aire;
    int w;
    int h;
};

void make_counter(SDL_Surface* grid);
int black_bro(Uint32* pixels, int curr_pixel, int* bro, int w, int size, char* visited);
int rec(Uint32* pixels, int curr_pixel, int w, int* small, int* big, int size, char* visited);
void collect_aire(struct box* curr_box, Uint32* pixels, int w, int size, char* visited);
