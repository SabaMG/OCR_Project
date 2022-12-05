/* 
 *
 * ocr
 */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "network.h"
#include "ocr.h"
#include "training.h"

#define N_LAYERS 3
#define N_NEURONS_INPUT 784
#define N_NEURONS_HIDDEN_1 32
#define N_NEURONS_OUTPUT 10
#define DEFAULT_EPOCHS 5
#define DEFAULT_LEARNING_RATE 0.1

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

	if (d_flag) {
		if (network_path != NULL)
			load_weights(network_path, network);

		training(network, training_folder_path, learning_rate, v_flag);
	}
	free_network(network, 3);

	return EXIT_SUCCESS;
}
