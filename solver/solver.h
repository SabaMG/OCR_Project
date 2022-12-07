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
int is_complete(int grid[][SIZE]);
void possibility_array(int grid[][SIZE], int poss_array[][SIZE]);
void order_array(int grid[][SIZE], int order_array[][2]);
void copy_array(int original[][SIZE], int copy [][SIZE]);
int backtracking_algo(int original[][SIZE],
		      int grid[][SIZE],
		      int order[][2], size_t order_i);
void solve(int grid[][SIZE], int result[][SIZE]);
