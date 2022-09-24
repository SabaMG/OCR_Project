#pragma once
#define SIZE 9

int check_row(int number,
	      int grid[][SIZE],
              size_t i, size_t j);
int check_col(int number,
              int grid[][SIZE],
              size_t i, size_t j);
int check_squ(int number,
              int grid[][SIZE],
              size_t i, size_t j);
int well_place(int number, int grid[][SIZE], size_t i, size_t j);
int is_solve(int grid[][SIZE]);
void possibility_array(int grid[][SIZE], int poss_array[][SIZE]);
void order_array(int grid[][SIZE], int order_array[][2]);
