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
    char s[21];
    while(fgets(s, 21, stream) != NULL)
    {
	if(strcmp(s, "\n") != 0)
	{
	    for(size_t k = 0; k < 19; ++k)
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
    for(size_t i = 0; i < 16; ++i)
    {
	for(size_t j = 0; j < 16; ++j)
	{
	    fputc(to_char(grid[i][j]), stream);
	    if(j == 3 || j == 6 || j == 9)
		fputc(' ', stream);
	}
	fputc('\n', stream);
	if(i == 3 || i == 6 || i == 9)
	    fputc('\n', stream);
    }

    //close stream
    fclose(stream);
}

int to_int(char c)
{
    if(c == '.')
	return 0;
    else if(c == 'A')
        return 10;
    else if(c == 'B')
        return 11;
    else if(c == 'C')
        return 12;
    else if(c == 'D')
        return 13;
    else if(c == 'E')
        return 14;
    else if(c == 'F')
        return 15;
    return c - 48;
}

char to_char(int i)
{
    char c = 0; 
    if(i == 10)
        c = 'A';
    else if(i == 11)
        c = 'B';
    else if(i == 12)
        c = 'C';
    else if(i == 13)
        c = 'D';
    else if(i == 14)
        c = 'E';
    else if(i == 15)
        c = 'F';
    else
        c = i + 48;
    return c;
}
