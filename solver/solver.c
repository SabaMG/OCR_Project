#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "function.h"

void exit_help()
{
    char help[] =   "solver\n"
		    "Usage: sudoku_file\n";

    errx(EXIT_FAILURE, "%s", help);
}

int main(int argc,char** argv)
{
    if (argc == 1)
    {
	exit_help();
    }
    
    unsigned int grid[GRID_SIZE][GRID_SIZE];
    convert_grid(grid, argv[1]);
    for(size_t i = 0; i < 9; ++i)
    {
	for (size_t j = 0; j < 9; ++j)
	{
	    printf("%i|", grid[i][j]);
	}
	printf("\n");
    }

    return EXIT_SUCCESS;
}
