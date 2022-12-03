#pragma once

void collect_boxes_coor(int width, int heigth, int coor[][2], size_t nb_cell);
SDL_Surface* create_number(char* number, int font_size, SDL_Color color, char* font_path);
void write_on_grid(SDL_Surface* grid, SDL_Surface* number, int x, int y);
