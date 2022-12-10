#include <stdio.h>
#include "hexa_solver.h"

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
    for(size_t k = (i / 4) * 4; k < (i / 4) * 4 + 4; ++k)
    {
	for(size_t l = (j / 4) * 4; l <  (j / 4) * 4 + 4; ++l)
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
    for(size_t i = 0; i < 16; ++i)
    {
	for(size_t j = 0; j < 16; ++j)
	{
	    if(!well_place(grid[i][j], grid, i, j))
	    {
		return 0;
	    }
	}
    }
    return 1;
}

int is_complete(int grid[][SIZE])
{
    for(size_t i = 0; i < 16; ++i)
    {
	for(size_t j = 0; j < 16; ++j)
	{
	    //printf("[%zu, %zu]", i ,j);
	    if(grid[i][j] == 0)
	    {
		return 0;
	    }
	}
    }
    return 1;
}

// this function make a array of int, when the case equal to:
// 0: if there is a number
// 1-16: if there is no number, representing the number
//	possibility in this case
void possibility_array(int grid[][SIZE], int poss_array[][SIZE])
{
    for(size_t i = 0; i < 16; ++i)
    {
	for(size_t j = 0; j < 16; ++j)
	{
	    if(grid[i][j] == 0) 
	    {
		for(int k = 1; k < 17; ++k)
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
    for(int k = 1; k < 17; ++k)
    {
	for(size_t i = 0; i < 16; ++i)
	{
	    for(size_t j = 0; j < 16; ++j)
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

// This function copy the array original in the array copy
void copy_array(int original[][SIZE], int copy[][SIZE])
{
    for(size_t i = 0; i < 16; ++i)
    {
	for(size_t j = 0; j < 16; ++j)
	{
	    copy[i][j] = original[i][j];
	}
    }
}

// This function solve the original grid
int backtracking_algo(int original[][SIZE], 
		      int grid[][SIZE],
		      int order[][2], size_t order_i)
{   
    // copy the grid
    int copy[SIZE][SIZE];
    copy_array(grid, copy);

    // coordinate of case to complete
    size_t i = order[order_i][0];
    size_t j = order[order_i][1];
    for(int k = 1; k < 16; ++k)
    {
	// if number k is well place 
        if(well_place(k, copy, i, j))
        {
	    // modify copy and check if it complete
	    copy[i][j] = k;
	    if(is_complete(copy))
	    {
	        // if copy is solve modify original
	        if(is_solve(copy))
	        {
		    original[i][j] = k;
		    return 1;
		}
	    }
	    else
	    {
		int son = backtracking_algo(original, copy, order, order_i + 1);
	        // if son solve grid modify original
		if(son)
		{
		    original[i][j] = k;
		    return 1;
		}
	    }
	}
    }

    return 0;
}

// This function solve directly the grid sudoku
// And put result grid in result array
void solve(int grid[][SIZE], int result[][SIZE])
{
    // create possibility array for grid
    int array[SIZE][SIZE] = {};
    possibility_array(grid, array);
    
    // create order array for solve grid
    int order[SIZE * SIZE][2] = {};
    order_array(array, order);
    
    // copy grid in result 
    copy_array(grid, result);
    
    // solve grid
    int res = backtracking_algo(result, grid, order, 0);

    // sudoku can't be solved
    if(!res)
	printf("error, impossible sudoku");
}
