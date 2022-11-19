/* 
 * ocr
 */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "impl.h"
#include "file.h"
#include "neural_network.h"

// Network stuff
#define N_LAYERS 3
#define N_NEURONS_INPUT 784
#define N_NEURONS_HIDDEN_1 32
#define N_NEURONS_OUTPUT 10
#define DEFAULT_EPOCHS 5
#define DEFAULT_LEARNING_RATE 0.1

int build_image_folder_paths(int path_length, char *training_folder_path, char* a_paths[]) {
	// Try to open training directory
	DIR* training_dir = opendir(training_folder_path);
	if (!training_dir) {
		if (ENOENT == errno)
			errx(EXIT_FAILURE, "Directory '%s' does not exist.", training_folder_path);
		else
			errx(EXIT_FAILURE, "Failed to open '%s' directory.", training_folder_path);
	}

	// Get training_folder_path length
	path_length += 2; // Add '/' and the number

	// Build each path
	for (size_t i = 0; i < 10; i++) {
		// Build directory path
		char *path = (char*)malloc(path_length);
		strcat(path, training_folder_path);
		strcat(path, "/");
		char *str_i = (char*)malloc(2);
		sprintf(str_i, "%zu", i);
		strcat(path, str_i);
		free(str_i);
		// Check if folder at new path exists
		if (!path) {
			if (ENOENT == errno)
				errx(EXIT_FAILURE, "build_image_folder_paths: Directory '%s' does not exist.", path);
			else
				errx(EXIT_FAILURE, "build_image_folder_paths: Failed to open '%s' directory.", path);
		}
		//printf("build_image_folder_paths: Path for '%zu' successfully added.\n", i);
		a_paths[i] = path;
	}

	closedir(training_dir);
	return 0;
}

int IMAGES_NUMBER = 60000;

int get_next_image_path(char **image_path, int *path_length, char *dir_path, int current_indexes[], int curr_number) {

	int next_index = current_indexes[curr_number];
	char str_curr_number[2];
	str_curr_number[0] = (char)('0' + curr_number);
	str_curr_number[1] = '\0';

	int can_path = 0;
	while (next_index < IMAGES_NUMBER && can_path == 0) {
		// Build next test image path
		*image_path = (char *)malloc(*path_length + 25);
		strcat(*image_path, dir_path);
		strcat(*image_path, "/");
		strcat(*image_path, str_curr_number);
		strcat(*image_path, "_");
		char *str_i = (char *)malloc(10);
		sprintf(str_i, "%i", next_index); 
		strcat(*image_path, str_i);
		strcat(*image_path, ".png");
		free(str_i);
		// Test if image_path exists
		//printf("get_next_image_path: Try to open '%s'.\n", image_path);
		if (access(*image_path, F_OK) != -1) {
			//printf("\033[0;32m");
			//printf("get_next_image_path: Successfully get next_image '%s'.\n", *image_path);
			//printf("\033[0m");
			can_path = 1;
		}
		else {
			//printf("\033[0;31m");
			//printf("get_next_image_path: Failed to open '%s'.\n", *image_path);
			//printf("\033[0m");
			free(*image_path);
		}
		next_index++;
	}
	current_indexes[curr_number] = next_index;
	return 0;
}

int training(int v_flag, int e, double learning_rate, int path_length, char* image_folder_paths[], int current_indexes[], Layer network[]) {
	// TESTING
	int good = 0;
	int bad = 0;
	// Get random seed
	srand(time(NULL));

	char *path = NULL;
	char **image_path = &path;

	Uint32 pixel;
	Uint8 r, g, b, v = 0;

	// Array that will contain grey scale values
	double training_inputs[784];
	double expected_outputs[10];

	// Run training for a number of epochs
	for (int i = 0; i < e; i++) {
		int r_int = 0;
		r_int = (int)(rand() % 10); // Get a number between 0 and 9

		// Get random image path
		if (get_next_image_path(image_path, &path_length, image_folder_paths[r_int], current_indexes, r_int) != 0)
			errx(EXIT_FAILURE, "Error in get_next_image_path");

		// Loading the image as a surface
		SDL_Surface *surface = IMG_Load(*image_path);
		if (surface == NULL)
			errx(EXIT_FAILURE, "training: Error when load image '%s'.\n", *image_path);
		//printf("training: Load image '%s'.\n", *image_path);

		// Standardizes the formats
		// Not very useful here but ok
		surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

		//printf("w = %i, h = %i\n", surface->w, surface->h);
		// Loop through pixels
		Uint32* pixels = surface->pixels;
		for (int y = 0; y < surface->h; y++) {
			for (int x = 0; x < surface->w; x++) {
				pixel = pixels[y * surface->w + x];
				r = pixel >> 16 & 0xFF;
				g = pixel >> 8 & 0xFF;
				b = pixel & 0xFF;
				v = (r + g + b) / 3;
				training_inputs[y * surface->w + x] = (double)v;

			}
		}

		// Build expected_outputs array
		for (size_t i = 0; i < 10; i++) {
			expected_outputs[i] = 0;
		}
		expected_outputs[r_int] = 1;
		//printf("Expected number: %i\n", r_int);

		// Feed neurons
		feed_forward_pass(network, training_inputs);
		//feed_forward_pass2(network, training_inputs);

		// Brief evaluation of the output layer
		int best = 0;
		//printf("output layer values: [ ");
		for (size_t q = 0; q < 10; q++) {
			//printf("%f ", network[2].neurons[q].value);
			if (network[2].neurons[best].value < network[2].neurons[q].value)
				best = q;
		}
		//printf("]\n");
		//printf("Ouput number: %i\n", best);

		if (best == r_int) { // :)
			if (v_flag) {
				printf("\033[0;32m");
				printf("Expected: %i\tPredicted: %i\n", r_int, best);
				printf("\033[0m");
			}
			good++;
		}
		else { // :(
			if (v_flag) {
				printf("\033[0;31m");
				printf("Expected: %i\tPredicted: %i\t(%s)\n", r_int, best, *image_path);
				printf("\033[0m");
			}
			bad++;
		}

		//backpropagation_pass2(network, expected_outputs, learning_rate);

		// Do backpropagation
		backpropagation_pass(network, expected_outputs);

		// Update neurons
		update_network(network, learning_rate);
		

		SDL_FreeSurface(surface);
	}

	printf("\n");
	printf("Corrects: %i (%.2f%%) \t Incorrects: %i (%.2f%%)\t (%i rounds)\n", good, ((float)good / (float)e) * 100, bad, ((float)bad / (float)e) * 100, e);

	return 0;
}

int main(int argc, char **argv) {

	int epochs = DEFAULT_EPOCHS;
	double learning_rate = DEFAULT_LEARNING_RATE;

	// Parsing arguments
	int d_flag = 0;
	int v_flag = 0;
	char *training_folder_path;
	char *network_path = NULL;
	char *arg;

	int i = 1;
	while (i < argc) {
		arg = argv[i];
		if (strcmp(arg, "-d") == 0) {
			d_flag = 1;
			i++;
			if (i < argc)
				training_folder_path = argv[i];
			else
				errx(EXIT_FAILURE, "Option '-d' requires an argument.");
		}
		else if (strcmp(arg, "-v") == 0) {
			v_flag = 1;
			i++;
		}
		else if (strcmp(arg, "-l") == 0) {
			i++;
			if (i < argc)
				network_path = argv[i];
			else
				errx(EXIT_FAILURE, "Option '-l' requires an argument.");
		}
		else if (strcmp(arg, "-e") == 0) {
			i++;
			if (i < argc)
				epochs = atoi(argv[i]);
			else
				errx(EXIT_FAILURE, "Option -e requires an argument.");
		}
		else if (strcmp(arg, "-r") == 0) {
			i++;
			if (i < argc)
				learning_rate = atof(argv[i]);
			else
				errx(EXIT_FAILURE, "Option -r requires an argument.");
		}
		else
			errx(EXIT_FAILURE, "Unknown option '%s'.", arg);
		i++;
	}	

	// Create network
	size_t N_NEURONS_ARRAY[] = {N_NEURONS_INPUT, N_NEURONS_HIDDEN_1, N_NEURONS_OUTPUT};
	size_t sizes_inputs[] = {784, 784, 32};

	Layer network[N_LAYERS];
	generate_network(network, N_LAYERS, N_NEURONS_ARRAY, sizes_inputs);


	//print_layer(network, n_layers);

	if (d_flag) {
		// Array contening all 10 image folder paths
		int path_length = 0;
		while (training_folder_path[path_length] != 0)
			path_length++;

		char* image_folder_paths[10]; 
		int current_indexes[10] = {0};
		build_image_folder_paths(path_length, training_folder_path, image_folder_paths);

		if (network_path != NULL)
			load_weights(network_path, network);

		training(v_flag, epochs, learning_rate, path_length, image_folder_paths, current_indexes, network);

		save_weights("network.save", network, N_LAYERS);
	}

	return EXIT_SUCCESS;
}
