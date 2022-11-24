#pragma once

// define structs for neuron network
struct neuron {
    size_t size;
    double bias;
	double value; // value of the neuron
	double delta; // for the gradient loss function
    double* weights;
    double* begin;
    double* end;
};

struct layer {
    size_t size;
    struct neuron *neurons;
    struct neuron *begin;
    struct neuron *end;
};

// define type for structs
typedef struct layer Layer;
typedef struct neuron Neuron;

void load_weights(char *path, Layer l[]);

void save_weights(char *path, Layer l[], size_t nb_layer);

double getRandom();

void print_layer(Layer network[], size_t n_layers);

void free_network(Layer network[], size_t n_layers);

void generate_network(Layer network[], size_t n_layers, size_t n_neurons[], size_t n_inputs[]);
