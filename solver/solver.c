#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "read_file.h"
#include "backtracking.h"

void exit_help()
{
    char help[] =   "solver\n"
		    "Usage:"
		    "--read filename\n"
		    "--check filename number i j";

    errx(EXIT_FAILURE, "%s", help);
}

size_t c1(char c)
{
    return c - 48;
}

void test_read(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[GRID_SIZE][GRID_SIZE];
    convert_grid(grid, argv[2]);
    for(size_t i = 0; i < 9; ++i)
    {
	for (size_t j = 0; j < 9; ++j)
	{
	    printf("%i|", grid[i][j]);
	}
	printf("\n");
    }
}

void test_check(int argc, char** argv)
{
    if(argc != 6)
	exit_help();
    
    int num = atoi(argv[3]);
    size_t i = atoi(argv[4]);
    size_t j = atoi(argv[5]);

    print_grid(argv[2]);
    printf("\n");

    int grid[GRID_SIZE][GRID_SIZE];
    convert_grid(grid, argv[2]);

    int r = check_row(num, grid, i);
    printf("line %zu contain %i : %i\n", i, num, r);
    r = check_col(num, grid, j);
    printf("col %zu contain %i : %i\n", j, num, r);
    r = check_squ(num, grid, i, j);
    printf("[%zu, %zu] contain %i : %i\n", i, j, num,  r);
}

int main(int argc,char** argv)
{
    if (argc == 1)
    {
	exit_help();
    }
    
    if(strcmp(argv[1], "--read") == 0)
    {
	test_read(argc, argv);
    }
    else if(strcmp(argv[1], "--check") == 0)
    {
	test_check(argc, argv);
    }
    else
    {
	exit_help();
    }

    return EXIT_SUCCESS;
}
