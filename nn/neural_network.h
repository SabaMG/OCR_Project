#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "impl.h"

void softmax(size_t layer_size, Neuron neurons[]);

void feed_forward_pass(Layer network[], double inputs[]);

void backpropagation_pass(Layer network[], double expected_outputs[]);

void update_network(Layer network[], double learning_rate);
