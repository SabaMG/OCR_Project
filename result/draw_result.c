#include <stdio.h>
#include <stddef.h>
#include <math.h>

// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "draw_result.h"

#define GRID_SIZE 534

void collect_boxes_coor(int width, int heigth, int coor[][2], size_t nb_cell)
{
    int d_w = width / (int)nb_cell;
    int d_h = heigth / (int)nb_cell;
    int start_w = d_w / 2;
    int start_h = d_h / 2;
    size_t curr_cell = 0;

    for(int i = start_w; i < width; i += d_w)
    {
	for(int j = start_h; j < heigth; j += d_h)
	{
	    coor[curr_cell][0] = i;
	    coor[curr_cell][1] = j;
	    curr_cell += 1;
	}
    }
}

// This function create a surface
// And write in this surface the char number with size font size and color
SDL_Surface* create_number(char* number, int font_size, SDL_Color color, char* font_path)
{
    // Init ttf
    TTF_Init();
    
    // Create font variable with font arial.ttf and size font_size
    TTF_Font* font = TTF_OpenFont(font_path, font_size);
    if(font == NULL)
    {
	printf("font null : %s\n", TTF_GetError());
    }

    // Create surface with number of color color
    SDL_Surface* number_surface = TTF_RenderText_Solid(font, number, color);
    if(number_surface == NULL)
	printf("surface null\n");

    // close font and quit ttf
    TTF_CloseFont(font);
    TTF_Quit();

    return number_surface;
}

// This function place surface number at position x, y in surface grid
// and free number
void write_on_grid(SDL_Surface* grid, SDL_Surface* number, int x, int y)
{
    // rect to center surface in coordinate x et y
    SDL_Rect rect;
    rect.w = number->w;
    rect.h = number->h;
    rect.x = x - rect.w / 2;
    rect.y = y - rect.h / 2;

    // merge surface
    SDL_BlitSurface(number, NULL, grid, &rect);

    // Free surface number
    SDL_FreeSurface(number);
}
