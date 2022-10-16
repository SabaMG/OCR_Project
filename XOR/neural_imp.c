#include "neural_imp.h"

// This function make a leayer of random neurons
void make_layer(size_t nb_inputs, size_t nb_neurons, double layer[])
{
    time_t t;
    srand((unsigned)time(&t)); 
    for(size_t i = 0; i < nb_neurons; ++i)
    {
	for(size_t j = 0; j < nb_inputs; ++j)
	{
	    layer[i * (nb_inputs + 1) + j] = ((double)(rand() % 1000)) / 1000;
	}

	layer[i * (nb_inputs + 1) + nb_inputs] = ((double)(rand() % 1000))/1000;
    }
}

void print_layer(size_t nb_inputs, size_t nb_neurons, double layer[])
{
    for(size_t i = 0; i < nb_neurons; ++i)
    {
	printf("N%zu\n", i + 1);
	for(size_t j = 0; j < nb_inputs; ++j)
	{
	    printf("  w%zu=%f", j + 1, layer[i * (nb_inputs + 1) + j]);
	}
	printf("  b=%f\n", layer[i * (nb_inputs + 1) + nb_inputs]);
    }
}
