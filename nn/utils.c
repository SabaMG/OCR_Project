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
/*
double dSigmoid(double x) {
	return x * (1 - x);
}
*/

/* return a random number between 0 and 1
	used for initializing weights */
double getRandom() {
	//srand(time(NULL));
	return ((double)rand()) / ((double)RAND_MAX) * 2 - 1;
}
