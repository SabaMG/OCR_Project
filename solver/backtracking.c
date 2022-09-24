#include <stdio.h>
#include "backtracking.h"

int check_row(int number,
	      int grid[][GRID_SIZE],
	      size_t i)
{
    for(size_t k = 0; k < GRID_SIZE; ++k)
    {
	if(grid[i][k] == number)
	{
	    return 1;
	}
    }

    return 0;
}

int check_col(int number,
	      int grid[][GRID_SIZE],
	      size_t j)
{
    for(size_t k = 0; k < GRID_SIZE; ++k)
    {
	if(grid[k][j] == number)
	{
	    return 1;
	}
    }

    return 0;
}

int check_squ(int number,
	      int grid[][GRID_SIZE],
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
