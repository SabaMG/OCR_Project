#include <stdio.h>
#include <stdlib.h>

#include "solver.h"
#include "file.h"

int main(int argc,char** argv)
{
    // convert file in array
    int grid[SIZE][SIZE] = {};
    read_grid(grid, argv[1]);
    
    int result[SIZE][SIZE] = {};
    for(size_t i = 0; i < SIZE; ++i)
    {
	for(size_t j = 0; j < SIZE; ++j)
	{
	    printf("%i ", result[i][j]);
	}
	printf("\n");
    }
    printf("\n\n");

    solve(grid, result);

    for(size_t i = 0; i < SIZE; ++i)
    {
	for(size_t j = 0; j < SIZE; ++j)
	{
	    printf("%i ", result[i][j]);
	}
	printf("\n");
    }

    return EXIT_SUCCESS;
}
