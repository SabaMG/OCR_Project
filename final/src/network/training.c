#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"
#include "ocr.h"
#include "network.h"

void shuffle_samples(double ***samples, int *number_of_files, int v_flag) {
	if(v_flag)
		printf("shuffle_samples: START SHUFFLE\n");
	int n = 0;
	for (size_t i = 0; i < 10; i++) {
		n = number_of_files[i];
		if (n > 1) {
			int j;
			for (j = 0; j < n - 1; j++) {
				size_t k = j + rand() / (RAND_MAX / (n - j) + 1);
				double *t = samples[i][k];
				samples[i][k] = samples[i][j];
				samples[i][j] = t;
			}
		}
	}
	if(v_flag)
		printf("shuffle_samples: FINISH SHUFFLE\n");
}

// Evaluate network
int evaluate(Layer network[], double ***samples, double results[][10], int *current_indexes, size_t n_sample, int v_flag) {
	if(v_flag)
		printf("evaluate: START EVALUATION\n");
	srand(time(NULL));
	int r_int = 0;
	int error = 0;
	unsigned last = 2;
	for (size_t i = 0; i < n_sample; i++)
	{
		r_int = (int)(rand() % 10);
		/*
		float dist = 0;
		feed_forward(network, samples[r_int][current_indexes[r_int]]);
		for (size_t i = 0; i < n_sample; i++) {
			for (unsigned j = 0; j < network[last].size; j++) {
				dist += pow(network[last].neurons[j].value - results[r_int][j], 2);
			}
			error += sqrt(dist);
		}
		*/
		int best = 0;
		feed_forward(network, samples[r_int][current_indexes[r_int]]);
		for (size_t q = 0; q < 10; q++) {
			if (network[2].neurons[best].value < network[2].neurons[q].value)
				best = q;
		}
		if (best != r_int)
			error++;
	}
	if (v_flag) {
		printf("evaluate: FINISH EVALUATION\n");
		printf("evaluate: Reset current_indexes\n");
	}
	for (size_t i = 0; i < 10; i++)
		current_indexes[i] = 0;

	return error;
}

// Train network
float train(Layer network[], double ***samples, double results[][10], int *number_of_files, int *current_indexes, double learning_rate, size_t total, int v_flag) {
	shuffle_samples(samples, number_of_files, v_flag);
	srand(time(NULL));
	int r_int = 0;

	//for (int i = 0; i < 2000; i++) {
	//	printf("i = %i\n", i);
	for (size_t j = 1; j < total; j++) {
		printf("\rtrain: %.2f%% (%zu/%zu)", ((float)j / total) * 100, j, total);
		fflush(stdout);
		r_int = (int)(rand() % 10);
		if (current_indexes[r_int] < number_of_files[r_int]) {
			feed_forward(network, samples[r_int][current_indexes[r_int]]);
			backpropagation(network, results[r_int]);
			update_network(network, learning_rate);
			current_indexes[r_int]++;
		}
	}
	//}
	printf("\n");

	// Reset current_indexes
	if (v_flag)
		printf("train: Reset current_indexes array\n");
	for (size_t i = 0; i < 10; i++)
		current_indexes[i] = 0;

	return evaluate(network, samples, results, current_indexes, 80, v_flag); // TODO: put 100 in variable
																	  //return 0.f;
}

void free_samples(double ***samples, int *number_of_files) {
	printf("free_samples: START FREE MEMORY\n");
	for (size_t i = 0; i < 10; i++) {
		for (int j = 0; j < number_of_files[i]; j++) {
			free(samples[i][j]);
		}
		free(samples[i]);
	}
	free(samples);
	printf("free_samples: FINISH FREE MEMORY\n");
}

void training(Layer *network, char *training_folder_path, int epochs, double learning_rate, int v_flag, gpointer user_data) {
	ProgramData *data = user_data;
	// Build each 10 image folder path
	int path_length = 0;
	while (training_folder_path[path_length] != 0)
		path_length++;
	char *image_folder_paths[10];
	DIR * training_dir;
	struct dirent * entry;
	training_dir = opendir(training_folder_path);
	int k = 0;
	while ((entry = readdir(training_dir)) != NULL) {
		if ((char)entry->d_name[0] >= '0' && (char)entry->d_name[0] <= '9') {
			char *path = (char *)calloc(path_length + 3, sizeof(char));
			strcat(path, training_folder_path);
			strcat(path, "/");
			strcat(path, entry->d_name);
			image_folder_paths[k] = path;
			if (v_flag)
				printf("training: Path for '%s' successfully added.\n", entry->d_name);
			k++;
		}
	}
	closedir(training_dir);

	// Counts the number of files in each image folder
	int number_of_files[10] = {0};
	int total = 0;
	for (size_t i = 0; i < 10; i++) {
		DIR * dirp;
		struct dirent * entry;
		dirp = opendir(image_folder_paths[i]);
		while ((entry = readdir(dirp)) != NULL) {
			//if (entry->d_type == DT_REG) {
				number_of_files[i]++;
				total++;
			//}	
		}
		if (v_flag)
			printf("training: %i files in folder '%s'\n", number_of_files[i], image_folder_paths[i]);
		closedir(dirp);
	}
	printf("training: %i files in total\n", total);

	if (v_flag)
		printf("training: START ALLOCATE MEMORY\n");
	double ***samples = (double ***)malloc(10 * sizeof(double **));
	for (size_t i = 0; i < 10; i++) {
		double **folder = (double **)malloc(number_of_files[i] * sizeof(double *));
		samples[i] = folder;
		for (int j = 0; j < number_of_files[i]; j++) {
			double *image = (double *)malloc(784 * sizeof(double));
			folder[j] = image;
		}
	}
	if(v_flag)
		printf("training: FINISH ALLOCATE MEMORY\n");

	if(v_flag)
		printf("training: START CREATE SAMPLES\n");
	Uint32 pixel;
	Uint8 r, g, b, v = 0;
	size_t j = 0;
	float counter = 0;
	char *image_path = NULL;
	for (size_t i = 0; i < 10; i++) {
		DIR * dirp;
		struct dirent * entry;
		dirp = opendir(image_folder_paths[i]);
		j = 0;
		while ((entry = readdir(dirp)) != NULL) {
			//if (entry->d_type == DT_REG && strlen(entry->d_name) > 2) {
				if ('g' == entry->d_name[strlen(entry->d_name) - 1]) {
					// Build image path;
					image_path = (char *)calloc(path_length + 25, sizeof(char));
					strcat(image_path, image_folder_paths[i]);
					strcat(image_path, "/");
					strcat(image_path, entry->d_name);
					printf("\rProcessing images: %.2f%% (%s)", (counter / total) * 100, image_path);
					fflush(stdout);

					//printf("learning: Process image %s\n", image_path);
					SDL_Surface *surface = IMG_Load(image_path);

					if (surface == NULL)
						errx(EXIT_FAILURE, "training: Error when load image '%s' (SDL).\n", image_path);

					surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

					Uint32* pixels = surface->pixels;
					for (int y = 0; y < surface->h; y++) {
						for (int x = 0; x < surface->w; x++) {
							pixel = pixels[y * surface->w + x];
							r = pixel >> 16 & 0xFF;
							g = pixel >> 8 & 0xFF;
							b = pixel & 0xFF;
							v = (r + g + b) / 3;
							samples[i][j][y * surface->w + x] = (double)v;
						}
					}
					SDL_FreeSurface(surface);
					j++;
					counter++;
					free(image_path);
				}
			//}
		}
		free(image_folder_paths[i]);
		closedir(dirp);
	}
	printf("\n");
	if(v_flag)
		printf("training: FINISH CREATE SAMPLES\n");

	// Build results arrays
	double results[10][10];
	for (size_t i = 0; i < 10; i++) {
		for (size_t t = 0; t < 10; t++) {
			if (t == i)
				results[i][t] = (double)(1);
			else
				results[i][t] = (double)(0);
		}
	}

	int current_indexes[10] = {0};
	clock_t chrono = clock();
	int error = evaluate(network, samples, results, current_indexes, 80, v_flag);
	int bestError = error;
	int goal = 5;
	printf("training: START LEARNING\n");
	/*
	printf("----------------------------------------------------------\n");
	printf("\tEpoch 0: %i / %i\n", (100 - error), 100);
	printf("----------------------------------------------------------\n");
	*/
	int e = 1;
	//while (error > goal)
	while (e <= epochs) {

		if (data->net.is_training == 0 || !gtk_widget_get_visible(GTK_WIDGET(data->ui.nn_dialog))) {
			printf("stop training !!\n");
			// Stop training because of button clicked
			data->net.is_training = 0;
			data->net.training_retval = 0; // Stop before to finish
			free_samples(samples, number_of_files);
			g_thread_exit(NULL);
		}

		error = train(network, samples, results, number_of_files, current_indexes, learning_rate, total, v_flag);

		error = error < goal ? goal : error;
		printf("----------------------------------------------------------\n");
		if (error < bestError) {
			bestError = error;
			char *res_path = NULL;
			save_weights("./networks/exported_network_", network, 3, &res_path);
			printf("\033[0;32m");
		}
		else
			printf("\033[0;31m");

		printf("\tEpoch %i: %i / %i\n", e, (80 - error), 80);
		printf("\033[0m");
		printf("----------------------------------------------------------\n");
		// Inc epoch
		e++;
	}
	printf("  - Time : %.6f seconds\n", (clock() - chrono) / 1000000.0F);
	printf("training: DONE TRAINING\n");

	free_samples(samples, number_of_files);
}

