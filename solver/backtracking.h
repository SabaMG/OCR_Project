#pragma once
#define GRID_SIZE 9

int check_row(int number,
	      int grid[][GRID_SIZE],
              size_t k);
int check_col(int number,
              int grid[][GRID_SIZE],
              size_t k);
int check_squ(int number,
              int grid[][GRID_SIZE],
              size_t i, size_t j);
