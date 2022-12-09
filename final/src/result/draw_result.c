#include <stdio.h>
#include <stddef.h>
#include <math.h>

// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "draw_result.h"

int min(int x, int y)
{
   if(x < y) 
       return x;
    return y;
}

// Copy surface
SDL_Surface *copie_surface(SDL_Surface *source)
{
    SDL_Surface* copie = SDL_CreateRGBSurface(0, source->w, source->h, 32,0,0,0,0);
    SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(source, NULL, copie, NULL);

    return copie;
}

// This function collect the boxe coordinates(usless)
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
SDL_Surface* create_number(char* number, SDL_Color color, TTF_Font* font)
{
    // Create surface with number of color color
    SDL_Surface* number_surface = TTF_RenderText_Solid(font, number, color);
    if(number_surface == NULL)
	printf("surface null\n");


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

// This function make the final surface grid with empty_grid:
// - in black init values
// - in green solution values
SDL_Surface* draw_empty(int grid[][SIZE], int result[][SIZE])
{
    // Init sdl window and image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    // load the empty grid and modify format
    SDL_Surface* init_grid = IMG_Load("empty_grid.jpg");
    SDL_Surface* empty_grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(init_grid);

    // coordinates
    int coor[][2]={{29,29},{29,88},{29,147},{29,206},{29,265},{29,324},{29,383},{29,442},
    {29,501},{88,29},{88,88},{88,147},{88,206},{88,265},{88,324},{88,383},{88,442},{88,501},
    {147,29},{147,88},{147,147},{147,206},{147,265},{147,324},{147,383},{147,442},{147,501},
    {206,29},{206,88},{206,147},{206,206},{206,265},{206,324},{206,383},{206,442},{206,501},
    {265,29},{265,88},{265,147},{265,206},{265,265},{265,324},{265,383},{265,442},{265,501},
    {324,29},{324,88},{324,147},{324,206},{324,265},{324,324},{324,383},{324,442},{324,501},
    {383,29},{383,88},{383,147},{383,206},{383,265},{383,324},{383,383},{383,442},{383,501},
    {442,29},{442,88},{442,147},{442,206},{442,265},{442,324},{442,383},{442,442},{442,501},
    {501,29},{501,88},{501,147},{501,206},{501,265},{501,324},{501,383},{501,442},{501,501}};

    // Init green and black colors
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color green = {34, 139, 34, 255};

    // Init ttf
    TTF_Init();
    
    // Create font variable with font arial.ttf and size font_size
    int font_size = (int)(min(empty_grid->w, empty_grid->h) / 9) * 0.90;
    TTF_Font* font = TTF_OpenFont("./assets/font.ttf", font_size);
    if(font == NULL) {
//	printf("font null : %s\n", TTF_GetError());
    }

    // write number on empty_grid surface
    for(size_t i = 0; i < SIZE; ++i)
    {
	for(size_t j = 0; j < SIZE; ++j)
	{
	    char str[] = {result[i][j] + 48, 0};
	    SDL_Surface* numb;

	    // result value (green)
	    if(grid[i][j] == 0)
	    {
		// make number surface
		numb = create_number(str, green, font);
	    }
	    // init value (black)
	    else
	    {
		// make number surface
		numb = create_number(str, black, font);
	    }
	    // write number surface on empty_grid surface
	    size_t c = j * SIZE + i;
	    write_on_grid(empty_grid, numb, coor[c][0], coor[c][1]);
	}
    }

    // close font and quit ttf
    TTF_CloseFont(font);
    TTF_Quit();

    // free surface
    return empty_grid;
}

// This function make the final surface grid with grid_image:
// - in black init values
// - in green solution values
SDL_Surface* draw_image(SDL_Surface* grid_image,
    int grid[][SIZE], int result[][SIZE], int coor[][2])
{
    // Init green colors
    SDL_Color green = {0, 126, 0, 255};

    // Init ttf
    TTF_Init();

    // surface result
    SDL_Surface* solved_grid = copie_surface(grid_image);
    
    // Create font variable with font arial.ttf and size font_size
    int font_size = (coor[1][1] - coor[0][1]) * 0.7;
//    printf("font size : %i\n",font_size);
    TTF_Font* font = TTF_OpenFont("./assets/font.ttf", font_size);
    if(font == NULL) {
	printf("draw_image(): font null : %s\n", TTF_GetError());
    }

    // write number on grid_image surface
    for(size_t i = 0; i < SIZE; ++i)
    {
	for(size_t j = 0; j < SIZE; ++j)
	{
	    char str[] = {result[i][j] + 48, 0};
	    SDL_Surface* numb;

	    // result value (green)
	    if(grid[i][j] == 0)
	    {
		// make number surface
		numb = create_number(str, green, font);
		// write number surface on grid_image surface
		size_t c = j * SIZE + i;
	        write_on_grid(solved_grid, numb, coor[c][0], coor[c][1]);
	    }
	}
    }

    // close font and quit ttf
    TTF_CloseFont(font);
    TTF_Quit();

    // free surface
    SDL_FreeSurface(grid_image);
    
    // return surface
    return solved_grid;
}
