#include <stdio.h>
#include <stddef.h>
#include "neural_imp.h"

int main(int argc, char** argv)
{
    // make a layer for xor
    size_t nb_inputs = 2;
    size_t nb_neurons = 2;
    double layer[(nb_inputs + 1) * nb_neurons];
    make_layer(nb_inputs, nb_neurons, layer);

    // print layer
    print_layer(nb_inputs, nb_neurons, layer);
}
