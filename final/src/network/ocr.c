#include <err.h>

#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "network.h"

#define PATH_MAX 200 // Because buggy on PIE

// Initialize ocr: load default network + set ui
void ocr_init(const char *network_path, Layer *network, GtkLabel* main_nn_path_label, GtkLabel* nn_nn_path_label) {
	int err = load_weights(network_path, network);
	printf("ocr_init(): Load network\n");

	if (err == 1)
		errx(1, "init_ocr: Failed to init ocr.\n");

	char cwd[PATH_MAX];
	char text[PATH_MAX] = {0};

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		strcat(text, cwd);
		strcat(text, "/");
		strcat(text, network_path);
	}
	else
		printf("init_ocr: Failed to get cwd.\n");
	gtk_label_set_text(main_nn_path_label, text);
	gtk_label_set_text(nn_nn_path_label, text);
}

/* return sigmoid(x)
	https://en.wikipedia.org/wiki/Sigmoid_function */
double sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

// Softmax function
void softmax(size_t layer_size, Neuron neurons[]) {
	double sum = 0;
	for (size_t i = 0; i < layer_size; i++)
		sum += exp(neurons[i].value); 

	sum = sum == 0 ? 1 : sum;
	for (size_t i = 0; i < layer_size; i++)
		neurons[i].value = exp(neurons[i].value) / sum;
}

// Forward pass
// The output layer is updated after pass
void feed_forward(Layer network[], double inputs[]) {

	// DEBUG
	printf("[\n");
	for (size_t i = 0; i < 28; i++) {
		for (size_t j = 0; j < 28; j++) {
			if (inputs[i * 28 + j] == 255)
				inputs[i * 28 + j] = 1;
			printf("%.f ", inputs[i * 28 + j]);
		}
		printf("\n");
	}
	printf("]\n");
	printf("\n");

	// Put inputs in neurons value
	// network[0] is the input layer
	for (size_t i = 0; i < network[0].size; i++) {
		network[0].neurons[i].value = inputs[i];
	}

	// Compute hidden layer
	// network[1] is the hidden layer
	for (size_t j = 0; j < network[1].size; j++) {
		// Put the bias of the precedent layer neuron in the value of the current hidden neuron
		network[1].neurons[j].value = network[0].neurons[j].bias;

		for (size_t k = 0; k < network[0].size; k++)
			network[1].neurons[j].value += network[0].neurons[k].weights[j] * network[0].neurons[k].value;
		network[1].neurons[j].value = sigmoid(network[1].neurons[j].value);
	}

	// Compute output layer
	// network[2] is the output layer
	for (size_t j = 0; j < network[2].size; j++) {
		network[2].neurons[j].value = network[1].neurons[j].bias;
		for (size_t k = 0; k < network[1].size; k++)
			network[2].neurons[j].value += network[1].neurons[k].weights[j] * network[1].neurons[k].value;
	}

	softmax(network[2].size, network[2].neurons);
}

// Backpropagation pass
// Calculate the gradient of the loss function
// Works because the number of neurons in the precedent layer > number of neurons of the current layer
void backpropagation(Layer network[], double expected_output[]) {

	for (size_t i = 0; i < network[2].size; i++) {
		double n_value = network[2].neurons[i].value;
		network[1].neurons[i].delta = n_value * (1 - n_value) * (n_value - expected_output[i]);
	}

	for (size_t i = 1; i > 0; i--) {
		for (size_t j = 0; j < network[i].size; j++) {
			network[i - 1].neurons[j].delta = 0;
			for (size_t k = 0; k < network[i + 1].size; k++)
				network[i - 1].neurons[j].delta += network[i].neurons[j].weights[k] * network[i].neurons[k].delta;
			network[i - 1].neurons[j].delta += network[i].neurons[j].value * (1 - network[i].neurons[j].value);
		}
	}
}

// Update weights and biases of a network
void update_network(Layer network[], double learning_rate) {
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < network[i].size; j++) {
			for (size_t k = 0; k < network[i + 1].size; k++)
				network[i].neurons[j].weights[k] -= learning_rate * network[i].neurons[j].value * network[i].neurons[k].delta;
		}

		for (size_t j = 0; j < network[i + 1].size; j++)
			network[i].neurons[j].bias -= learning_rate * network[i].neurons[j].delta;
	}
}

int ocr(Layer *network, SDL_Surface* sp, SDL_Surface** s_from_inputs) {
	//SDL_Surface *sp = SDL_ConvertSurfaceFormat(&surface, SDL_PIXELFORMAT_ARGB8888, 0);
	double *inputs = (double *)malloc(784 * sizeof(double));
	Uint32* pixels = sp->pixels;
	Uint32 pixel;
	Uint8 r, g, b, v = 0;
	//printf("[ ");
	for (int y = 0; y < sp->h; y++) {
		for (int x = 0; x < sp->w; x++) {
			pixel = pixels[y * sp->w + x];
			r = pixel >> 16 & 0xFF;
			g = pixel >> 8 & 0xFF;
			b = pixel & 0xFF;
			v = (r + g + b) / 3;
			if (v < 100)
				v = 0;
			else
				v = 255;
			//printf("%i ", v);
			inputs[y * sp->w + x] = (double)v;
			Uint32* n_pixels = (*s_from_inputs)->pixels;
			n_pixels[y * (*s_from_inputs)->w + x] = SDL_MapRGB((*s_from_inputs)->format, v, v, v);
			/*
			if (v < 100 ) 
				n_pixels[y * (*s_from_inputs)->w + x] = SDL_MapRGB((*s_from_inputs)->format, 0, 0, 0);
			else 
				n_pixels[y * (*s_from_inputs)->w + x] = SDL_MapRGB((*s_from_inputs)->format, 255, 255, 255);
				*/
		}
	//	printf("\n");
	}
	//printf(" ]\n");
	
	feed_forward(network, inputs);

	int best = 0;
	printf("[ ");
	for (size_t k = 0; k < 10; k++) {
		printf("%f ", network[2].neurons[k].value);
		if (network[2].neurons[best].value < network[2].neurons[k].value)
			best = k;
	}
	printf("]");
	printf("\n");

	free(inputs);
	//SDL_FreeSurface(surface);
	return best;
}
