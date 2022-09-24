#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_file.h"

void print_grid(char filename[])
{
    FILE* stream;
    char s[13];

    stream = fopen(filename, "r");

    if(stream == NULL)
    {
	printf ("file can't be open");
    }

    printf("In file :\n");

    while(fgets(s, 13, stream) != NULL)
    {
	printf("%s", s);
    }

    fclose(stream);
}

void convert_grid(int grid[][SIZE], char filename[])
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
		    grid[i][j] = convert(s[k]);
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

int convert(char c)
{
    if(c == '.')
	return 0;
    return c - 48;
}
