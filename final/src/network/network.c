#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <err.h>
#include <time.h>

#include "network.h"

// This function load weights from path file
int load_weights(const char *path, Layer l[]) {
    // open strea reader in stream
    FILE* stream;
    stream = fopen(path, "r");
    
    // check if file can be open
    if(stream == NULL) {
		return 1;
        //errx(1, "file can't be open");
    }
    
    // layer i and neuron j
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
	int z = -1;

    // read file char by char
    char c;
    char val_str[100] = "";
    double val;
    char * endPtr;
	size_t tmp = 0;
    while((c = fgetc(stream)) != EOF) {
        switch(c) {
            // next layer
            case ']':
                i += 1;
                j = 0;
                k = 0;
                break;
            // write bias +next neuron
            case '}':
                val = atof(val_str);
                val_str[0] = 0;
                l[i].neurons[j].bias = val;
                j += 1;
                k = 0;
                break;
            // write weight + Next weight
            case ';':
                val = atof(val_str);
                val_str[0] = 0;
				if (z != 2)
					l[i].neurons[j].weights[k] = val;
                k += 1;
                break;
            // write last weight
            case ')':
                val = atof(val_str);
                val_str[0] = 0;
				if (z != 2)
					l[i].neurons[j].weights[k] = val;
                break;
            default:
                if(c == '{' || c == '(')
                    break;
                if(c == '[') {
					z++;
					break;
				}
				if (c == '.')
					c = ',';
                strncat(val_str, &c, 1);
        }
    }
    // close stream reader
    fclose(stream);
	return 0;
}

int file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    int is_exist = 0;
    if (fp != NULL) {
        is_exist = 1;
        fclose(fp); // close the file
    }
    return is_exist;
}

// This function save weight in path file
int save_weights(char *path, Layer l[], size_t nb_layer, char **res_path) {
    // open stream write in stream
    FILE* stream = NULL;
    //stream = fopen(path, "w");
	int i = 10;
	char *save_path;
	do {
		char* buf = (char *)calloc((int)(log10(i)) + 1, sizeof(char));
		snprintf(buf, (int)log10(i) + 1, "%i", i);
		save_path = (char *)calloc(strlen(buf) + 40, sizeof(char));
		//strcat(save_path, "./networks/exported_network_");
		strcat(save_path, path);
		strcat(save_path, buf);
		strcat(save_path, ".net");
		free(buf);
		i += 10;
	}
	//while (access(save_path, F_OK) == 0 && i < 1000); // TODO: put 1000 in a variable
	while (file_exists(save_path) == 1 && i < 1000); // TODO: put 1000 in a variable
    // check if file can be open
	stream = fopen(save_path, "w");
    if(stream == NULL)
		return 1;
        //errx(EXIT_FAILURE, "save_weights: Failed to write in '%s'.\n", save_path);
    for(size_t i = 0; i < nb_layer; ++i) {
        fputc('[', stream);
        size_t nb_neuron = l[i].size;
        for(size_t j = 0; j < nb_neuron; ++j) {
            fputc('{', stream);
            fputc('(', stream);
            // Save weights
            size_t nb_weight = l[i].neurons[j].size;
            for(size_t k = 0; k < nb_weight; ++k) {
                // convert double in string
                char str[100];
                sprintf(str, "%f", l[i].neurons[j].weights[k]);
				fputs(str, stream);
				//printf("%s\n", str);

                if (k < nb_weight - 1) {
                    fputc(';', stream);
                }
            }
            fputc(')', stream);
            // Save bias
            char str[100];
            sprintf(str, "%f", l[i].neurons[j].bias);
            fputs(str, stream);
            fputc('}', stream);
			//printf("do neuron %i of layer %i\n", j, i);

        }
        fputc(']', stream);
    }
	fflush(stream);
	fclose(stream);
	//printf("Network saved in '%s'\n", save_path);
	*res_path = save_path;
	//free(save_path);
	return 0;
}


/* return a random number between 0 and 1
	used for initializing weights */
double getRandom() {
	return ((double)rand()) / ((double)RAND_MAX) * 2 - 1;
}

// This function print the layer l
void print_layer(Layer l[], size_t size) {
	for(size_t i = 0; i < size; ++i) {
//			printf("Layer%zu:\n", i);
		size_t nb_neuron = l[i].size;

		//printf("size: %i\n", nb_neuron);
		for(size_t j = 0; j < nb_neuron; ++j) {
			if (j < 5)
				printf("\nbias of the %ith neuron: %f\n",j, l[i].neurons[j].bias);
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
	//printf("free_network(): FINISH\n");
}

// This function generate a neural network with rand values
void generate_network(Layer l[], size_t layers, size_t sizes_neurons[], size_t sizes_inputs[]) {

	//int o = 0;
	// Generate layer i 
	srand(time(NULL));
	for(size_t i = 0; i < layers; ++i) {
		l[i].size = sizes_neurons[i];
		l[i].neurons = calloc(l[i].size, sizeof(struct neuron));
		//printf("gen(): %i neurons in layer %i\n", l[i].size, i);
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
			//printf("gen(): %i weights for neuron %i\n", n.size, j);
			n.weights = calloc(n.size, sizeof(double));
			n.begin = n.weights;
			n.end = n.begin + n.size;

    		// generate weights k of neuron j
			/*
			for (double* k = n.begin; k < n.end; ++k) {
				*k = getRandom();
			}
			*/
			for (size_t k = 0; k < n.size; k++) {
				n.weights[k] = getRandom();
			}

			n.bias = getRandom(); // init bias
			//printf("%f ", n.bias);
			n.value = 0; // init value
			n.delta = 0; // init delta
						 // make link between layer i and neuron j
			*p++ = n;
		}
	}
	//printf("generate_network(): FINISH\n");
}

