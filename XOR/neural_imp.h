#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

void make_layer(size_t nb_inputs, size_t nb_neurons, double layer[]);
void print_layer(size_t nb_inputs, size_t nb_neurons, double layer[]);
