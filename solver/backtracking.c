#include <stdio.h>
#include "backtracking.h"

// This function return 1 if number is
// present in row i else 0
int check_row(int number,
	      int grid[][SIZE],
	      size_t i)
{
    for(size_t k = 0; k < SIZE; ++k)
    {
	if(grid[i][k] == number)
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
	      size_t j)
{
    for(size_t k = 0; k < SIZE; ++k)
    {
	if(grid[k][j] == number)
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
		    int presence = check_row(k, grid, i) +
				   check_col(k, grid, j) +
				   check_squ(k, grid, i, j);
		    if(presence == 0)
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
void order_array(int poss_array[][SIZE], int ord_array[][SIZE])
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
		    ord_array[i][j] = order;
		    ++order;
		}
	    }
	}
    }
}
