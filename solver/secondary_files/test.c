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

void print_array(int grid[][SIZE])
{
    for(size_t i = 0; i < 9; ++i)
    {
	for ( size_t j = 0; j < 9; ++j)
	{
	    if(j % 3 == 0)
		printf(" ");
	    if(grid[i][j] != 0)
		printf("[%i]", grid[i][j]);
	    else
		printf("[ ]");
	}
	if((i + 1) % 3 == 0)
	    printf("\n");
	printf("\n");
    }
}

void test_read(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {0};
    convert_grid(grid, argv[2]);
    print_array(grid);
}

void test_check(int argc, char** argv)
{
    if(argc != 6)
	exit_help();
    
    int num = atoi(argv[3]);
    size_t i = atoi(argv[4]);
    size_t j = atoi(argv[5]);

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);
    print_array(grid);
    printf("\n");

    int r = check_row(num, grid, i, j);
    printf("line %zu contain %i : %i\n", i, num, r);
    r = check_col(num, grid, i, j);
    printf("col %zu contain %i : %i\n", j, num, r);
    r = check_squ(num, grid, i, j);
    printf("[%zu, %zu] contain %i : %i\n", i, j, num,  r);
}

void test_possibility(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);
    
    int array[SIZE][SIZE] = {};
    possibility_array(grid, array);
    printf("Possibility array :\n");
    print_array(array);
}

void test_order(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);
    
    int array[SIZE][SIZE] = {};
    possibility_array(grid, array);

    int order[SIZE * SIZE][2] = {};
    order_array(array, order);
    print_array(grid);
    printf("\nOrder Array :\n");
    for(size_t i = 0; i < SIZE * SIZE; ++i)
    {
	grid[order[i][0]][order[i][1]] = -1;
    }
    print_array(grid);
}

void test_well_place(int argc, char** argv)
{
    if(argc != 6)
	exit_help();
    
    int num = atoi(argv[3]);
    size_t i = atoi(argv[4]);
    size_t j = atoi(argv[5]);

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);
    print_array(grid);
    printf("\n");

    printf("is number %i is well place : %i\n",
	    num, well_place(num, grid, i, j));
}

void test_is_solve(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);

    if(is_solve(grid))
        printf("%s is solve\n", argv[2]);
    else
        printf("%s is not solve\n", argv[2]);
}

void test_complete(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);

    if(is_complete(grid))
        printf("%s is complete\n", argv[2]);
    else
        printf("%s is not complete\n", argv[2]);
}

void test_copy(int argc, char** argv)
{
    if(argc != 3)
	exit_help();

    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);

    int copy[SIZE][SIZE] = {};
    copy_array(grid, copy);
    
    copy[4][4] = 10;
    print_array(grid);
    printf("\n");
    print_array(copy);
}

void solve(int argc, char** argv)
{
    if(argc != 3)
	exit_help();
    
    // convert file in array
    int grid[SIZE][SIZE] = {};
    convert_grid(grid, argv[2]);
    
    // create possibility array for grid
    int array[SIZE][SIZE] = {};
    possibility_array(grid, array);
    
    // create order array for solve grid
    int order[SIZE * SIZE][2] = {};
    order_array(array, order);
    
    // create a copy of grid
    int copy[SIZE][SIZE] = {};
    copy_array(grid, copy);
    
    print_array(grid);
    int res = backtracking_algo(grid, copy, order, 0);
    if(res)
	printf("\ngrid is solve\n");
    else
	printf("\ngrid is not solve\n");
    print_array(grid);
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
    else if(strcmp(argv[1], "--poss") == 0)
    {
	test_possibility(argc, argv);
    }
    else if(strcmp(argv[1], "--ord") == 0)
    {
	test_order(argc, argv);
    }
    else if(strcmp(argv[1], "--is_solve") == 0)
    {
	test_is_solve(argc, argv);
    }
    else if(strcmp(argv[1], "--well") == 0)
    {
	test_well_place(argc, argv);
    }
    else if(strcmp(argv[1], "--is_complete") == 0)
    {
	test_complete(argc, argv);
    }
    else if(strcmp(argv[1], "--copy") == 0)
    {
	test_copy(argc, argv);
    }
    else if(strcmp(argv[1], "--solve") == 0)
    {
	solve(argc, argv);
    }
    else
    {
	exit_help();
    }

    return EXIT_SUCCESS;
}
