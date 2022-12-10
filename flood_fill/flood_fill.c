#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

// import SDL2 library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// import file
#include "flood_fill.h"
#include "queue.h"

int my_abs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

void color_grid(SDL_Surface* grid, char* M, int size)
{
    Uint32* pixels = grid->pixels;

    for(int i = 0 ; i < size; ++i)
    {
        if(M[i] == 1)
        {
            pixels[i] = 0xFFFF0000;
        }
    }
}

void write_mat(char* src, char* dest, int size)
{
    for(int i = 0; i < size; ++i)
    {
	if(src[i] != 0)
	{
	    dest[i] = src[i];
	}
    }
}

SDL_Surface* crop_grid (SDL_Surface* grid)
{
    Uint32* pixels = grid->pixels;
    // middle height of image
    int middle_h = 0;
    
    // coor of grid
    int start_grid = 0;
    int width_grid = 0;
    int height_grid = 0;

    char* M_grid = calloc(grid->w * grid->h, sizeof(char*));
    printf("w = %i , h = %i\n", grid->w, grid->h);

    for(int k = 0; k < 10; ++k)
    {
	middle_h = (grid->h * k/10) * grid->w;
	// check all pixel in the line
	for(int i = 0; i < grid->w; ++i)
	{
	    // check if it's a white pixel
	    if(pixels[middle_h + i] == 0xFFFFFFFF && M_grid[middle_h + i] == 0)
	    {
		char* M = calloc(grid->w * grid->h, sizeof(char));

		// flood fill for pixel middle_h + i
		int start = 0;
		int width = 0;
		int height = 0;
		collect_form(grid, middle_h + i, M, &start, &width, &height);

		int curr_delta = my_abs(width - height);
		if(curr_delta < 5)
		{
		    int delta = my_abs(width + height - width_grid - height_grid);
		    if(delta < 10)
		    {
			write_mat(M, M_grid, grid->w * grid->h);
		    }
		    else
		    {
		    	if(width + height > width_grid + height_grid)
			{
			    start_grid = start;
			    width_grid = width;
			    height_grid = height;
			    free(M_grid);
			    M_grid = calloc(grid->w * grid->h, sizeof(char));
			    write_mat(M, M_grid, grid->w * grid->h);
			}
		    }
		}

		free(M);
	    }
	}
    }

    extract_data(M_grid, grid->w, grid->h, &start_grid, &width_grid, &height_grid, 1);
    free(M_grid);

    // cut image with this coordinates
    SDL_Surface* croped_grid = SDL_CreateRGBSurface(0, width_grid + 40, height_grid + 40, 32,0,0,0,0);

    SDL_Rect crop_rect;
    crop_rect.x = start_grid % grid->w - 20;
    crop_rect.y = start_grid / grid->w - 20;
    crop_rect.w = width_grid + 40;
    crop_rect.h = height_grid + 40;
    
    SDL_BlitSurface(grid, &crop_rect, croped_grid, NULL);

    return croped_grid;
}

// this function extract the data from the matrix M
void extract_data(char* M, int w, int h, int* start_pix, int* width, int* height, int type)
{
    int size = w * h;
    int inv = (type + 1)%2;

    // start pixel
    int start = 0;
    while(start < size && M[start] == inv)
    {
        start += 1;
    }

    // end pixel
    int end = size - 1;
    while(end >= 0 && M[end] == inv)
    {
        end -= 1;
    }

    // left and right bornes
    int left = start % w;
    int right = end % w;
    int begin = start - left;
    int max_i = end / w - start / w;
    for(int i = 0; i < max_i; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            if(M[begin + (i * w + j)] == type)
            {
                if(j < left)
                {
                    left = j;
                }
                else if(j > right)
                {
                    right = j;
                }
            }
        }
    }

    *start_pix = (start / w) * w + (left % w);
    *width = right - left;
    *height = (end - start) / w;
}

// Collect all of the form in white beginning at pixel pos
void collect_form(SDL_Surface* grid, int pos, char* M, int* start, int* width, int* height)
{
    // Init some variables
    Uint32* pixels = grid->pixels;
    Uint32 white = 0xFFFFFFFF;
    int size = grid->w * grid->h;
    
    // start flood algo with begin pixel pos
    f_fill(pixels, pos, M, grid->w, size, white);
    
    // collect data from M
    extract_data(M, grid->w, grid->h, start, width, height, 1);

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
