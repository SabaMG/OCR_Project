#include <stdio.h>
#include <stdlib.h>

// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// include files
#include "flood_fill.h"

Uint32 color (Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return a << 24 | r << 16 | g << 8 | b;
}

void colored_case(Uint32* pixels, char* visited, int size)
{
    for (int i = 0; i < size; ++i)
    {
        if(visited[i])
            pixels[i] = color(0, 255, 0, 255);
    }
}

int clamp(int a, int b, int val)
{
    if (val < a)
        return a;
    if (val > b)
        return b;
    return val;
}

void copy(char* src, char* dest, int size)
{
    for(int i = 0; i < size; ++i)
    {
        dest[i] = src[i];
    }
}

void copy_box(struct box* src, struct box* dest)
{
    dest->pos = src->pos;
    dest->start = src->start;
    dest->end = src->end;
    dest->aire = src->aire;
    dest->w = src->w;
    dest->h = src->h;
}

int is_wigth(Uint32 pixel)
{
    return pixel == 0xFFFFFFFF;
}

void make_counter(SDL_Surface* grid)
{
    Uint32* pixels = grid->pixels;
    char* visited = calloc(grid->w * grid->h, sizeof(char));
    
    // Collect the middle of the image and nb of pixels
    int middle = (grid->h / 2) * grid->w + grid->w / 2;
    printf("%X\n", pixels[middle]);
    int size = grid->w*grid->h;
    
    // variable for copie
    struct box ref = {middle, 0, 0, 0, 0, 0};
    collect_aire(&ref, pixels, grid->w, size, visited);
    int t_sup = 2 * ref.aire;
    int t_inf = ref.aire/2;
    
    // search left
    search(pixels, &ref, size, grid->w, t_sup, t_inf, -ref.w, visited);
    // search right
    search(pixels, &ref, size, grid->w, t_sup, t_inf, ref.w, visited);
    // search top
    search(pixels, &ref, size, grid->w, t_sup, t_inf, (grid->w * ref.h),visited);
    // search down
    search(pixels, &ref, size, grid->w, t_sup, t_inf, -(grid->w * ref.h),visited);
    // search down

    free(visited);
}

void search(Uint32* pixels, struct box* ref, int size, int w, int t_sup, int t_inf, int shift, char* visited)
{
    int middle_case = ref->start + (ref->w/3) + (w * (ref->h/3));
    pixels[middle_case] = color(255, 0, 0, 255);
    
    // Search left
    struct box* curr = malloc(sizeof(struct box));
    copy_box(ref, curr);
    char* cop = calloc(size, sizeof(char));
    copy(visited, cop, size);
    while (curr->aire < t_sup && curr->aire > t_inf)
    {
        int begin = clamp(0, size, middle_case + shift);

        // make a copy of visited pixels
        copy(cop, visited, size);

        // Collect aire of case 
        curr->pos = begin;
        collect_aire(curr, pixels, w, size, cop);
        
        // middle of current case
        middle_case = curr->start + (curr->w/3) + (w*(curr->h/3));
    }
    colored_case(pixels, visited, size);
    free(cop);
    free(curr);
}

void collect_aire(struct box* curr_box, Uint32* pixels, int w, int size, char* visited)
{
    // get the aire of the box
    int start = size;
    int end = 0;
    int aire = rec(pixels, curr_box->pos, w, &start, &end, size, visited);
    // save infos
    curr_box->start = start;
    curr_box->end = end + 1 + w;
    curr_box->aire = aire;
    curr_box->w = (end - start) % w;
    curr_box->h = (end - start) / w;
}

int rec(Uint32* pixels, int curr_pixel, int w, int* left, int* right, int size, char* visited)
{
    // Store all black bro of curr_pixel
    int* bro = calloc(8, sizeof(int));
    if(bro == NULL)
    {
        printf("NOT ENOUCH MEMORY\n");
        return 0;
    }
    int nb_bro = black_bro(pixels, curr_pixel, bro, w, size, visited);

    // if no more black bro stop recusion
    if (nb_bro == 0)
    {
        free(bro);
        return 0;
    }

    // give the right size of number of bro
    bro = realloc(bro, nb_bro * sizeof(int));
    if(bro == NULL)
    {
        printf("NOT ENOUCH MEMORY\n");
        return 0;
    }
    
    
    // call rec function for all pixel
    int all_bro = nb_bro;
    for(int i = 0; i < nb_bro; ++i)
    {
        // Collect max values
        if(bro[i] < *left)
        {
            *left = bro[i];
        }
        else if(bro[i] > *right)
        {
            *right = bro[i];
        }
        //pixels[bro[i]] = color(0, 255, 0, 255);
        all_bro += rec(pixels, bro[i], w, left, right, size, visited);
    }

    free(bro);
    return all_bro;
}

int is_black(Uint32 pixel)
{
    return pixel == 0xFF000000;
}

int black_bro(Uint32* pixels, int curr_pixel, int* bro, int w, int size, char* visited)
{
    int nb = 0;
    for(int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (!(j == 0 && i == 0))
            {
                int coor = curr_pixel + (i * w) + j;
                if(coor > 0 && coor < size)
                {
                    if (is_black(pixels[coor]) && visited[coor] == 0)
                    {
                        visited[coor] = 1;
                        bro[nb] = coor;
                        nb += 1;
                    }
                }
            }
        }
    }

    return nb;
}
