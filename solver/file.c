#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"


void read_grid(int grid[][SIZE], char filename[])
{
    // open stream reader in stream
    FILE* stream;
    stream = fopen(filename, "r");
    
    // check if file can be open
    if(stream == NULL)
    {
	printf ("file can't be open");
    }
    
    // store file values un grid array
    size_t i = 0;
    size_t j = 0;
    char s[13];
    while(fgets(s, 13, stream) != NULL)
    {
	if(strcmp(s, "\n") != 0)
	{
	    for(size_t k = 0; k < 11; ++k)
	    {
		if(s[k] != ' ')
		{
		    grid[i][j] = to_int(s[k]);
		    ++j;
		}
	    }
	    ++i;
	    j = 0;
	}
    }
    
    // close stream reader
    fclose(stream);
}

void write_grid(int grid[][SIZE], char filename[])
{
    // open stream
    FILE* stream;
    strcat(filename, ".result");
    stream = fopen(filename, "w");
    
    // write array in file
    for(size_t i = 0; i < 9; ++i)
    {
	for(size_t j = 0; j < 9; ++j)
	{
	    fputc(to_char(grid[i][j]), stream);
	    if((j + 1) % 3 == 0)
		fputc(' ', stream);
	}
	fputc('\n', stream);
	if((i + 1)% 3 == 0)
	    fputc('\n', stream);
    }

    //close stream
    fclose(stream);
}

int to_int(char c)
{
    if(c == '.')
	return 0;
    return c - 48;
}

char to_char(int i)
{
    char c = i + 48;
    return c;
}
