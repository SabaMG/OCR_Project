#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "file.h"
#include "backtracking.h"

void exit_help()
{
    char help[] =   "solver\n"
		    "Usage:"
		    "--read filename\n"
		    "--check filename number i j";

    errx(EXIT_FAILURE, "%s", help);
}

int main(int argc,char** argv)
{
    if(argc != 2)
	exit_help();
    
    // convert file in array
    int grid[SIZE][SIZE] = {};
    read_grid(grid, argv[1]);
    
    // create possibility array for grid
    int array[SIZE][SIZE] = {};
    possibility_array(grid, array);
    
    // create order array for solve grid
    int order[SIZE * SIZE][2] = {};
    order_array(array, order);
    
    // create a copy of grid
    int copy[SIZE][SIZE] = {};
    copy_array(grid, copy);
    
    // solve grid
    int res = backtracking_algo(grid, copy, order, 0);

    // create file with grid solved
    if(res)
	write_grid(grid, argv[1]);

    return EXIT_SUCCESS;
}
