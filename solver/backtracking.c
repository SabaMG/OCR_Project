#include <stdio.h>
#include "backtracking.h"


// This function return 1 if number is
// present in row i else 0
int check_row(int number,
	      int grid[][SIZE],
	      size_t i, size_t j)
{
    for(size_t k = 0; k < SIZE; ++k)
    {
	if(j != k && grid[i][k] == number)
	{
	    return 1;
	}
    }

    return 0;
}

// This function return 1 if number is
// present in column j else 0
int check_col(int number,
	      int grid[][SIZE],
	      size_t i, size_t j)
{
    for(size_t k = 0; k < SIZE; ++k)
    {
	if(i != k && grid[k][j] == number)
	{
	    return 1;
	}
    }

    return 0;
}

// This function return 1 if number is 
// present in the big square [i,j] else 0
int check_squ(int number,
	      int grid[][SIZE],
	      size_t i, size_t j)
{
    for(size_t k = (i / 3) * 3; k < (i / 3) * 3 + 3; ++k)
    {
	for(size_t l = (j / 3) * 3; l <  (j / 3) * 3 + 3; ++l)
	{
	    if (!(k == i && l == j))
	    {
		if(grid[k][l] == number)
		    return 1;
	    }
	}
    }

    return 0;
}

// This function check if number can be place in case [i, j]
int well_place(int number, int grid[][SIZE], size_t i, size_t j)
{
    return check_row(number, grid, i, j) +
	   check_col(number, grid, i, j) +
	   check_squ(number, grid, i, j) == 0 ? 1 : 0;
}

// This function check if the grid is solve
int is_solve(int grid[][SIZE])
{
    for(size_t i = 0; i < 9; ++i)
    {
	for(size_t j = 0; j < 9; ++j)
	{
	    if(!well_place(grid[i][j], grid, i, j))
	    {
		printf("[%zu, %zu]", i, j);
		return 0;
	    }
	}
    }
    return 1;
}

// this function make a array of int, when the case equal to:
// 0: if there is a number
// 1-9: if there is no number, representing the number
//	possibility in this case
void possibility_array(int grid[][SIZE], int poss_array[][SIZE])
{
    for(size_t i = 0; i < 9; ++i)
    {
	for(size_t j = 0; j < 9; ++j)
	{
	    if(grid[i][j] == 0) 
	    {
		for(int k = 1; k < 10; ++k)
		{
		    if(well_place(k, grid, i, j))
		    {
			poss_array[i][j] += 1;
		    }
		}
	    }
	}
    }
}

// This function make an array of int,
// the case gives the filling order
void order_array(int poss_array[][SIZE], int ord_array[][2])
{
    int order = 1;
    for(int k = 1; k < 10; ++k)
    {
	for(size_t i = 0; i < 9; ++i)
	{
	    for(size_t j = 0; j < 9; ++j)
	    {
		if(poss_array[i][j] == k)
		{
		    ord_array[order - 1][0] = i;
		    ord_array[order - 1][1] = j;
		    ++order;
		}
	    }
	}
    }
}


void backtracking_algo(int grid[][SIZE], int order[][SIZE])
{

}
