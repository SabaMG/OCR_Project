/* 
 * Main file for the ocr neural network
 *
 * TODO
 * add implementation
 * do training with test
 * load training files
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

#define DEFAULT_EPOCHS_NUMBER 5;

int build_image_folder_paths(int path_length, char *training_folder_path, char* a_paths[]) {
	// Try to open training directory
	DIR* training_dir = opendir(training_folder_path);
	if (!training_dir) {
		if (ENOENT == errno) {
			errx(EXIT_FAILURE, "Directory '%s' does not exist.", training_folder_path);
		} else {
			errx(EXIT_FAILURE, "Failed to open '%s' directory.", training_folder_path);
		}
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
			if (ENOENT == errno) {
				errx(EXIT_FAILURE, "build_image_folder_paths: Directory '%s' does not exist.", path);
			} else {
				errx(EXIT_FAILURE, "build_image_folder_paths: Failed to open '%s' directory.", path);
			}
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
	//char *image_path = NULL;
	//char image_path[*path_length + 25];
	while (next_index < IMAGES_NUMBER && can_path == 0) {
		// Build next test image path
		//char *image_path = (char *)malloc(*path_length + 25);
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

int training(int e, int path_length, char* image_folder_paths[], int current_indexes[]) {
	// Get random seed
	srand(time(NULL));

	char *path = NULL;
	char **image_path = &path;

	// Run training for a number of epochs
	for (int i = 0; i < e; i++) {
		int r = (int)(rand() % 10); // Get a number between 0 and 9

		// Get random image path
		if (get_next_image_path(image_path, &path_length, image_folder_paths[r], current_indexes, r) != 0)
			errx(EXIT_FAILURE, "Error in get_next_image_path");

		//Loading the image as a surface
		SDL_Surface *img = IMG_Load(*image_path);
		if (img == NULL)
			errx(EXIT_FAILURE, "training: Error when load image '%s'.\n", *image_path);
		printf("training: Load image '%s'.\n", *image_path);

		//Standardizes the formats
		img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_FreeSurface(img);
	}

	return 0;
}

int main(int argc, char **argv) {
	/*
	// Create a Neural Network struct in memory
	// (The nn is a array of layers)
	// 3 layers: input, hidden, output
	size_t n_layers = 2;
	// input: 784 = 28 * 28 neurons
	size_t n_inputs_neurons = 784;
	// hidden: 784 = 28 * 28 neurons
	size_t n_hidden_neurons = 32;
	// output: 10 (one for each number)
	size_t n_output_neurons = 10;

	size_t a_inputs_neurons[] = {n_inputs_neurons, n_hidden_neurons};
	size_t a_hidden_neurons[] = {n_hidden_neurons, n_output_neurons};

	// Creates layers array struct
	Layer NN[n_layers];
	// Initialize the nn
	generate_network(NN, n_layers, a_hidden_neurons, a_inputs_neurons);
	print_layer(NN, n_layers);
	save_weights("save1.txt", NN, n_layers);
	*/
	char *training_folder_path;
	// Parsing arguments
	int d_flag = 0;
	char *e_value = NULL;
	char *arg;

	int i = 1;
	while (i < argc) {
		arg = argv[i];
		if (strcmp(arg, "-d") == 0) {
			d_flag = 1;
			i++;
			if (i < argc) {
				training_folder_path = argv[i];
			}
			else {
				errx(EXIT_FAILURE, "Option '-d' requires an argument.");
			}
		}
		else if (strcmp(arg, "-e") == 0) {
			i++;
			if (i < argc)
				e_value = argv[i];
			else
				errx(EXIT_FAILURE, "Option -e requires an argument.");
		}
		else {
			errx(EXIT_FAILURE, "Unknown option '%s'.", arg);
		}
		i++;
	}	

	if (d_flag) {
		// Array contening all 10 image folder paths
		int path_length = 0;
		while (training_folder_path[path_length] != 0) {
			path_length++;
		}

		char* image_folder_paths[10]; 
		int current_indexes[10] = {0};
		build_image_folder_paths(path_length, training_folder_path, image_folder_paths);

		int e = DEFAULT_EPOCHS_NUMBER;
		if (e_value != NULL) {
			e = atoi(e_value);
		}
		training(e, path_length, image_folder_paths, current_indexes);
	}

	return EXIT_SUCCESS;
}
