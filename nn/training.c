#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "ocr.h"
#include "network.h"

// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format) {
	Uint8 r, g, b;
	// Get r, g, b from the pixel.
	SDL_GetRGB(pixel_color, format, &r, &g, &b);
	// Determine average.
	Uint8 average = 255 - (0.3*r + 0.59*g + 0.11*b);
	// Set r, g, b to average.
	r = average;
	g = average;
	b = average;
	// Return new color.
	return SDL_MapRGB(format, r, g, b);
}

void surface_to_grayscale(SDL_Surface* surface) {
	// Get array of pixels.
	Uint32* pixels = surface->pixels;
	// Get len of the array.
	int len = surface->w * surface->h;
	// Get the format of the surface.
	SDL_PixelFormat* format = surface->format;
	// Lock the surface.
	if (SDL_LockSurface(surface) != 0)
		printf("error\n");
	//errx(EXIT_FAILURE, "%s", SDL_GetError());
	// Convert each pixel to gray.
	for (int i = 0; i < len; i++) {
		pixels[i] = pixel_to_grayscale(pixels[i], format);
	}
	// Unlock the surface.
	SDL_UnlockSurface(surface);
}

void shuffle(int array[], size_t n) {
	if (n > 1) {
		size_t i;
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

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
int evaluate(int ordre[], Layer network[], double ***samples, double results[][10], int *current_indexes, size_t n_sample, int v_flag) {
	if(v_flag)
		printf("evaluate: START EVALUATION\n");
	srand(time(NULL));
	//int r_int = 0;
	int error = 0;
	unsigned last = 2;
	int z = 0;
	printf("ordre des chiffres attendus: [ ");
	for (size_t i = 0; i < n_sample; i++) {
		//r_int = (int)(rand() % 10);
		int r_int = ordre[z];
		printf("%i, ", ordre[z]);
		z++;
		int best = 0;

		// j'affiche les chiffres
		/*
		   printf("[\n");
		   for (size_t i = 0; i < 28; i++) {
		   for (size_t j = 0; j < 28; j++) {
		   printf("%.f ", samples[r_int][current_indexes[r_int]][i * 28 + j]);
		//if (inputs[i * 28 + j] == 255)
		//inputs[i * 28 + j] = 1;
		}
		printf("\n");
		}
		printf("]\n");
		*/
		feed_forward(network, samples[r_int][current_indexes[r_int]]);
		for (size_t q = 0; q < 10; q++) {
			if (network[2].neurons[best].value < network[2].neurons[q].value)
				best = q;
		}
		if (best != r_int) {
			error++;
			//	printf("\033[0;31m");
			//	printf("Failed\n");
			//	printf("\033[0m");
		}
		//else {
		//	printf("\033[0;32m");
		//	printf("Success\n");
		//	printf("\033[0m");
		//}
	}
	printf("]\n");
	if (v_flag) {
		printf("evaluate: FINISH EVALUATION\n");
		printf("evaluate: Reset current_indexes\n");
	}
	for (size_t i = 0; i < 10; i++)
		current_indexes[i] = 0;

	return error;
	/*
	   float error = 0;
	   unsigned last = 2;
	   int r_int = 0;
	   srand(time(NULL));
	   for (size_t i = 0; i < n_sample; i++)
	   {
	   r_int = (int)(rand() % 10);
	   float dist = 0;
	   feed_forward(network, samples[r_int][current_indexes[r_int]]);
	   for (unsigned j = 0; j < network[last].size; j++)
	   dist += pow(network[2].neurons[j].value - r_int, 2);
	   error += sqrt(dist);
	   }
	   return error / (n_sample * 2);
	   */
}

void ocr(Layer network[], char *folder_path, int w_flag, int debug) {
	// je contruit la liste des chemins vers les images
	DIR * dirp;
	struct dirent * entry;
	dirp = opendir(folder_path);

	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_name[strlen(entry->d_name) - 1] == 'g') {
		//printf("ocr: load image %s\n", entry->d_name);

		char img_path[100] = {};
		strcat(img_path, folder_path);
		strcat(img_path, "/");
		strcat(img_path, entry->d_name);

		SDL_Surface *surface = IMG_Load(img_path);

		if (surface == NULL)
			errx(EXIT_FAILURE, "training: Error when load image '%s' (SDL: %s).\n", img_path, SDL_GetError());

		surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

		/*
		if (w_flag) {
			//printf("wflag, passage en niveau de gris blanc sur noir\n");
			surface_to_grayscale(surface);
		}
		*/

		double *image = (double *)malloc(784 * sizeof(double));
		Uint32 pixel;
		Uint8 r, g, b, v = 0;
	//	printf("w: %i | y: %i\n", surface->w, surface->h);
		Uint32* pixels = surface->pixels;
		for (int y = 0; y < surface->h; y++) {
			for (int x = 0; x < surface->w; x++) {
				pixel = pixels[y * surface->w + x];
				r = pixel >> 16 & 0xFF;
				g = pixel >> 8 & 0xFF;
				b = pixel & 0xFF;
				v = (r + g + b) / 3;
				if (v < 6)
					v = 0;
				image[y * surface->h + x] = v;
			}
		}

		// DEBUG
		// j'affiche les chiffres
		if (debug) {
			printf("[\n");
			for (size_t i = 0; i < 28; i++) {
				for (size_t j = 0; j < 28; j++) {
					printf("%.f ", image[i * 28 + j]);
				}
				printf("\n");
			}
			printf("]\n");
		}

		feed_forward(network, image);

		// Get out
		int out = 0;
		printf("[ ");
		for (size_t q = 0; q < 10; q++) {
			if (network[2].neurons[out].value < network[2].neurons[q].value)
				out = q;
		}
		printf("\n");
		switch(out) {
			case 0:
				out = 9;
				break;
			case 1:
				out = 0;
				break;
			case 2:
				out = 7;
				break;
			case 3:
				out = 6;
				break;
			case 4:
				out = 1;
				break;
			case 5:
				out = 8;
				break;
			case 6:
				out = 4;
				break;
			case 7:
				out = 3;
				break;
			case 8:
				out = 2;
				break;
			case 9:
				out = 5;
				break;
		}

		printf("%s\t===>\t%i\n", entry->d_name, out);

		SDL_FreeSurface(surface);
		}
	}
}

// Train network
int train(int ordre[], Layer network[], double ***samples, double results[][10], int *number_of_files, int *current_indexes, double learning_rate, size_t total, int v_flag, int n_sample) {
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

	return evaluate(ordre, network, samples, results, current_indexes, n_sample, v_flag); // TODO: put 100 in variable
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

void training(Layer *network, char *training_folder_path, double learning_rate, int v_flag, int w_flag) {
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

	// si le set est trop gros, j'en eleve a chaque dossier
	/*
	   if (total > 8000) {
	   printf("training: too big ! -> set to 8000 files\n");
	   printf("on retire %i images a chaque dossier\n", ((total - 8000) / 10));
	   for (size_t i = 0; i < 10; i++) {
	   number_of_files[i] = number_of_files[i] - ((total - 8000) / 10);
	//printf("%i images dans le dossier %i\n", number_of_files[i], i);
	}
	}
	*/

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
			//while ((entry = readdir(dirp)) != NULL && counter < 800) {
			//if (entry->d_type == DT_REG && strlen(entry->d_name) > 2) {
			if ('g' == entry->d_name[strlen(entry->d_name) - 1]) {
				// Build image path;
				image_path = (char *)calloc(path_length + 100, sizeof(char));
				strcat(image_path, image_folder_paths[i]);
				strcat(image_path, "/");
				strcat(image_path, entry->d_name);
				printf("\rProcessing images: %.2f%% (%s)", (counter / total) * 100, image_path);
				fflush(stdout);

				//printf("learning: Process image %s\n", image_path);
				SDL_Surface *surface = IMG_Load(image_path);

				if (surface == NULL)
					errx(EXIT_FAILURE, "training: Error when load image '%s' (SDL: %s).\n", image_path, SDL_GetError());

				surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

				/*
				if (w_flag) {
					//printf("wflag, passage en niveau de gris blanc sur noir\n");
					surface_to_grayscale(surface);
				}
				*/

				Uint32* pixels = surface->pixels;
				for (int y = 0; y < surface->h; y++) {
					for (int x = 0; x < surface->w; x++) {
						pixel = pixels[y * surface->w + x];
						r = pixel >> 16 & 0xFF;
						g = pixel >> 8 & 0xFF;
						b = pixel & 0xFF;
						v = (r + g + b) / 3;
						samples[i][j][y * surface->h + x] = (double)v;
					}
				}
				SDL_FreeSurface(surface);
				j++;
				//printf("bug\n");
				counter++;
				free(image_path);
			}
			//}
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
	shuffle_samples(samples, number_of_files, v_flag);
	//float error = evaluate(network, samples, results, current_indexes, 50, v_flag);
	int ordre[100];
	//srand(time(NULL));
	for (size_t i = 0; i < 100; i++) {
		ordre[i] = (int)(rand() % 10);
	}
	shuffle(ordre, 100);

	int n_sample = 100;

	int error = evaluate(ordre, network, samples, results, current_indexes, n_sample, v_flag);

	float bestError = error;
	//int goal = 5;
	printf("training: START LEARNING\n");
	/*
	   printf("----------------------------------------------------------\n");
	   printf("\tEpoch 0: %i / %i\n", (100 - error), 100);
	   printf("----------------------------------------------------------\n");
	   */
	int epoch = 0;
	int je_continue = 1;
	//while (error > goal)
	int h = 0;
	while (h < 100) {
		h++;
		//	shuffle_samples(samples, number_of_files, v_flag);
		epoch++;

		// je melange l'ordre pour l'evaluation
		/*
		   for (size_t i = 0; i < 100; i++) {
		   ordre[i] = (int)(rand() % 10);
		   }
		   */
		shuffle(ordre, 100);

		error = train(ordre, network, samples, results, number_of_files, current_indexes, learning_rate, total, v_flag, n_sample);
		//error = train(network, samples, results, number_of_files, current_indexes, 0.3, 50, v_flag);
		//error = error < goal ? goal : error;
		//printf("STATUS : %d%%\n", (int) ((1 - error) / (1 - goal) * 100));
		//printf("\033[0;34m");
		//printf("\033[0m");
		//if (error < bestError) {
		//printf("----------------------------------------------------------\n");
		//if (error < bestError) {
		if (error <= 30) {
			printf("\033[0;32m");
			printf("epoch %i: %i / %i\n", epoch, (n_sample - error), 100);
			printf("\033[0m");
			bestError = error;
			char* chemin;
			//if (bestError == 10) {
			chemin = save_weights("network.save", network, 3);
			printf("sauvegarde du reseau gagnant vers %s\n", chemin);
			// je reprends l'ancien reseau
			load_weights(chemin, network);
			printf("chargement du reseau gagnant depuis %s\n", chemin);
			// je refais l'evaluation
			printf("lancement de la 2eme evaluation\n");
			int res = evaluate(ordre, network, samples, results, current_indexes, n_sample, v_flag);
			int good = (100 - res);
			// je print le score de l'evaluation
			printf("score de l'evaluation 1: %i\n", (100 - error));
			printf("score de l'evaluation 2: %i\n", (100 - res));
			// je teste la fiabilite de l'evalution en changement l'ordre sur 100 evaluations
			size_t o = 0;
			int min = good;
			int max = good;
			int tot = 0;
			int nres = 0;
			int ngood = 0;
			while (o < 10) {
				/*
				   for (size_t i = 0; i < 100; i++) {
				   ordre[i] = (int)(rand() % 10);
				   }
				   */
				shuffle(ordre, 100);
				printf("ordre: [ ");
				for (size_t i = 0; i < 100; i++) {
					printf("%i ", ordre[i]);
				}
				printf("]\n");
				nres = evaluate(ordre, network, samples, results, current_indexes, n_sample, v_flag);
				ngood = 100 - nres;
				tot += ngood;
				if (ngood < min)
					min = ngood;
				if (ngood > max)
					max = ngood;
				if (nres < res)
					printf("\033[0;32m");
				else
					printf("\033[0;31m");
				printf("score de l'evaluation %i: %i\n", o, ngood);
				printf("\033[0m");
				o++;
				//sleep(1);
			}
			printf("depart = %i | min = %i | max = %i | diffToMin = %i | diffToMax = %i | moy = %f\n", good, min, max, good - min, max - good, (float)(tot / 10));
			// j'arrete la bouche
			//break;
			//}
		}
		else {
			printf("\033[0;31m");
			printf("epoch %i: %i / %i\n", epoch, (n_sample - error), 100);
			printf("\033[0m");
		}
		//printf("----------------------------------------------------------\n");
	}
	printf("  - Time : %.6f seconds\n", (clock() - chrono) / 1000000.0F);
	printf("training: DONE TRAINING\n");

	free_samples(samples, number_of_files);
	}

