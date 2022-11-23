#include <stdio.h>
#include <stdlib.h>
#include "impl.h"
#include "utils.h"

// This function print the layer l
void print_layer(Layer l[], size_t size) {
	for(size_t i = 0; i < size; ++i) {
		printf("Layer%zu:\n", i);
		size_t nb_neuron = l[i].size;

		for(size_t j = 0; j < nb_neuron; ++j) {
			printf("    bias : %f\n", l[i].neurons[j].bias);
			size_t nb_weight = l[i].neurons[j].size;

			for(size_t k = 0; k < nb_weight; ++k)
				printf("    weight : %f\n", l[i].neurons[j].weights[k]);

			printf("    value : %f\n", l[i].neurons[j].value);
			printf("    delta : %f\n", l[i].neurons[j].delta);
			printf("\n");
		}
	}
}

// This function free the memorie store for layers and neurons
void free_network(Layer l[], size_t size) {
	for(size_t i = 0; i < size; ++i) {
		for(Neuron* p = l[i].begin; p < l[i].end; ++p)
			free(p->weights);
		free(l[i].neurons);
	}
}

// This function generate a neural network with rand values
void generate_network(Layer l[], size_t layers, size_t sizes_neurons[], size_t sizes_inputs[]) {

	//int o = 0;
	// Generate layer i 
	for(size_t i = 0; i < layers; ++i) {
		l[i].size = sizes_neurons[i];
		l[i].neurons = calloc(l[i].size, sizeof(struct neuron));
		l[i].begin = l[i].neurons;
		l[i].end = l[i].begin + l[i].size;
		struct neuron * p = l[i].begin;

		// generate neuron j of layer i
		for(size_t j = 0; j < sizes_neurons[i]; ++j) {
			struct neuron n;
			if (i < layers - 1)
				n.size = sizes_neurons[i + 1];
			else
				n.size = 0;
		//	n.size = sizes_inputs[i];
			n.weights = calloc(n.size, sizeof(double));
			n.begin = n.weights;
			n.end = n.begin + n.size;

    		// generate weights k of neuron j
			for (double* k = n.begin; k < n.end; ++k) {
				*k = getRandom();
			//	*k = (0.5*(rand()/(double)(RAND_MAX)));
				//if (o%2)
				//	*k = -*k;

				//o++;
			}

			n.bias = getRandom(); // init bias
			n.value = 0; // init value
			n.delta = 0; // init delta
						 // make link between layer i and neuron j
			*p++ = n;
		}
	}
}

