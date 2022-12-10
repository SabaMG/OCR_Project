#pragma once

#include <stdio.h>

#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "network.h"

void ocr_init(const char *network_path, Layer *network, GtkLabel* main_nn_path_label, GtkLabel* nn_nn_path_label);

double sigmoid(double x);

void softmax(size_t layer_size, Neuron neurons[]);

void feed_forward(Layer network[], double inputs[]);

void backpropagation(Layer network[], double expected_outputs[]);

void update_network(Layer network[], double learning_rate);

int ocr(Layer *network, SDL_Surface *surface, SDL_Surface** n_from_inputs);
