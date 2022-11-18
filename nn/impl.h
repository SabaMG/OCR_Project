#pragma once

// define structs for neuron network
struct neuron
{
    size_t size;
    double bias;
    double* weights;
    double* begin;
    double* end;
};
struct layer
{
    size_t size;
    struct neuron *neurons;
    struct neuron *begin;
    struct neuron *end;
};

// define type for structs
typedef struct layer Layer;
typedef struct neuron Neuron;

void print_layer(Layer l[], size_t size);
void free_network(Layer l[], size_t size);
void generate_network(Layer l[],
                      size_t layers,
                      size_t neurons[],
                      size_t inputs[]);

