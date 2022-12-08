#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

// import SDL2 library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "flood_fill.h"
#include "queue.h"

void put_color(Uint32* pixels, char* M, int size, Uint32 color)
{
    for(int i = 0; i < size; ++i)
    {
        if(M[i] == 1)
        {
            pixels[i] = color;
        }
    }
}

int pos(SDL_Surface* grid, int middle_h, int w)
{
    Uint32* pixels = grid->pixels;

    for(int i = 0; i < w; ++i)
    {
        if(pixels[middle_h + i] == 0xFFFFFFFF)
        {
            return middle_h + i;
        }
    }

    return middle_h;
}

void collect_coor(SDL_Surface* grid, int pos, int* start, int* width, int* height)
{
    // Init some variables
    Uint32* pixels = grid->pixels;
    Uint32 white = 0xFFFFFFFF;
    int size = grid->w * grid->h;
    char* M = calloc(size, sizeof(char));
    
    f_fill(pixels, pos, M, grid->w, size, white);

    put_color(pixels, M, size, 0xFFFF0000);

    free(M);
}

// This function run the flood_fill algo and write res in matrix M
void f_fill(Uint32* pixels, int begin_pixel, char* M, int w, int size, Uint32 color)
{
    // create a queue
    struct queue* q = new_queue();

    // enqueue and marck first pixel
    push(q, begin_pixel);
    M[begin_pixel] = 1;

    while(!is_empty(q))
    {
        int curr = pop(q);

        // collect case next to curr
        int next[4] = {};
        int nb_next = next_to(pixels, curr, color, w, size, next);

        // en queue each case no visited
        for(int i = 0; i < nb_next; ++i)
        {
            // not visited
            if(M[next[i]] == 0)
            {
                M[next[i]] = 1;
                push(q, next[i]);
            }
        }
    }

    destroy(q);
}

// This function collect all the coordinate of case of color next to curr
int next_to(Uint32* pixels, int curr, Uint32 color, int w, int size, int next[4])
{
    int i = 0;

    // right case
    if((curr % w) + 1 < w && pixels[curr + 1] == color)
    {
        next[i] = curr + 1;
        i += 1;
    }

    // left case
    if((curr % w) - 1 > 0 && pixels[curr - 1] == color)
    {
        next[i] = curr - 1;
        i += 1;
    }

    // hupper case
    if(curr - w > 0 && pixels[curr - w] == color)
    {
        next[i] = curr - w;
        i += 1;
    }

    // downer case
    if(curr + w < size && pixels[curr + w] == color)
    {
        next[i] = curr + w;
        i += 1;
    }

    return i;
}
