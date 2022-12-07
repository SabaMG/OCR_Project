#pragma once

#define SIZE 9

SDL_Surface * copie_surface(SDL_Surface *a_copier);
void collect_boxes_coor(int width, int heigth, int coor[][2], size_t nb_cell);
SDL_Surface* create_number(char* number, SDL_Color color, TTF_Font* font);
void write_on_grid(SDL_Surface* grid, SDL_Surface* number, int x, int y);
SDL_Surface* draw_empty(int grid[][SIZE], int result[][SIZE]);
SDL_Surface* draw_image(SDL_Surface* grid_image, int grid[][SIZE], int result[][SIZE], int coor[][2]);
