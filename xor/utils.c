#include <stdlib.h>
#include <stdio.h>
#include  <string.h>
#include <math.h>
#include "utils.h"

/* return sigmoid(x)
	https://en.wikipedia.org/wiki/Sigmoid_function */
double sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

/* return the derivative of sigmoid(x) */
double dSigmoid(double x) {
	return x * (1 - x);
}

/* return a random number between 0 and 1
	used for initializing weights */
double getRandom() {
	return ((double)rand()) / ((double)RAND_MAX);
}

/* shuffle int array
   based on Ben Pfaff's Writings: https://benpfaff.org/writings/clc/shuffle.html */ 
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

/* initWeights initialize weights of hidden neurones and of output nodes
   and initialize bias
	set weights at random values */
int initWeights(
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double outputLayerBias[]
		) {

	// Set hiddenWeights to random values.
	for (size_t i = 0; i < nInputs; i++) {
		for (size_t j = 0; j < nHiddenNodes; j++) {
			hiddenWeights[i][j] = getRandom();
		}
	}

	// Set outputWeights to random values.
	for (size_t i = 0; i < nHiddenNodes; i++) {
		for (size_t j = 0; j < nOutputs; j++) {
			outputWeights[i][j] = getRandom();
		}
	}

	// Set outputLayerBias to random values.
	// TODO: do a init bias function ?
	for (size_t i = 0; i < nOutputs; i++) {
		outputLayerBias[i] = getRandom();
	}
	return 0;
}

/* load weights */
int load_weights(char *path) {
	FILE* ptr;
	char ch;

	ptr = fopen(path, "r"); /* open file in reading mode */
	if (NULL == ptr) /* return if error */
		return 1;

	do { /* read file character at once */
		ch = fgetc(ptr);
		printf("%c", ch);
} while (ch != EOF); /* stop if end of file */

fclose(ptr); /* close file */
return 0;
}

/* save weights */

