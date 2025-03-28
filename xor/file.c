#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "impl.h"

// This function load weights from path file
void load_weights(char *path, Layer l[])
{
    // open stream reader in stream
    FILE* stream;
    stream = fopen(path, "r");
    
    // check if file can be open
    if(stream == NULL)
    {
        errx(1, "file can't be open");
    }
    
    // layer i and neuron j
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    // read file char by char
    char c;
    char val_str[100] = "";
    double val;
    while((c = fgetc(stream)) != -1)
    {
        switch(c)
        {
            // next layer
            case ']':
                i += 1;
                j = 0;
                k = 0;
                break;
            // write bias +next neuron
            case '}':
                val = atof(val_str);
                val_str[0] = 0;
                l[i].neurons[j].bias = val;
                j += 1;
                k = 0;
                break;
            // write weight + Next weight
            case ',':
                val = atof(val_str);
                val_str[0] = 0;
                l[i].neurons[j].weights[k] = val;
                k += 1;
                break;
            // write last weight
            case ')':
                val = atof(val_str);
                val_str[0] = 0;
                l[i].neurons[j].weights[k] = val;
                break;
            default:
                if(c == '[' || c == '{' || c == '(')
                    break;
                strncat(val_str, &c, 1);
        }
    }
    
    // close stream reader
    fclose(stream);
}

// This function save weight in path file
void save_weights(char *path, Layer l[], size_t nb_layer)
{
    // open stream write in stream
    FILE* stream;
    stream = fopen(path, "w");

    // check if file can be open
    if(stream == NULL)
    {
        errx(1, "file can't be open");
    }

    for(size_t i = 0; i < nb_layer; ++i)
    {
        fputc('[', stream);
        size_t nb_neuron = l[i].size;
        for(size_t j = 0; j < nb_neuron; ++j)
        {
            fputc('{', stream);
            fputc('(', stream);
            // Save weights
            size_t nb_weight = l[i].neurons[j].size;
            for(size_t k = 0; k < nb_weight; ++k)
            {
                // convert double in string
                char str[100];
                sprintf(str, "%f", l[i].neurons[j].weights[k]);
                fputs(str, stream);

                if (k < nb_weight - 1)
                {
                    fputc(',', stream);
                }
            }
            fputc(')', stream);
            // Save bias
            char str[100];
            sprintf(str, "%f", l[i].neurons[j].bias);
            fputs(str, stream);
            fputc('}', stream);

        }
        fputc(']', stream);
    }
}

