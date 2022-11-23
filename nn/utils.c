#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "impl.h"

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
	//srand(time(NULL));
	return ((double)rand()) / ((double)RAND_MAX) * 2 - 1;
}

/* shuffle int array
   based on Ben Pfaff's Writings: https://benpfaff.org/writings/clc/shuffle.html */ 
void shuffle(int array[], size_t n) {
	srand(time(NULL));
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
