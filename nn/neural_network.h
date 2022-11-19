#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "impl.h"
extern const int nInputs;
extern const int nHiddenNodes;
extern const int nOutputs;
extern const int nTrainingSets;

void softmax(size_t layer_size, Neuron neurons[]);

void feed_forward_pass(
		Layer network[],
		double inputs[]
		//double hiddenLayer[],
		//double outputLayer[]
);

void backpropagation_pass(
		Layer network[],
		double expected_outputs[]
		//double learning_rate
		//double hiddenLayer[],
		//double outputLayer[]
		);

void update_network(Layer network[], double learning_rate);
