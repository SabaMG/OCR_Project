#pragma once

#define SIZE 9

void collect_boxes_coor(int width, int heigth, int coor[][2], size_t nb_cell);
SDL_Surface* create_number(char* number, SDL_Color color, TTF_Font* font);
void write_on_grid(SDL_Surface* grid, SDL_Surface* number, int x, int y);
void make_grid_empty(SDL_Surface* empty_grid, int grid[][SIZE], int result[][SIZE], int coor[][2], char* font_path, int font_size);
