/*
	Author: Charles-Antoine Leger

	Mini reseau de neurones capable d'apprendre la fonction
	OU EXCLUSIF (XOR).
	
	Les poids des noeuds du reseau peuvent etre sauvegardes
	et charges depuis un fichier specifie.

	On utilise la fonction sigmoid pour ce reseau.

	xor:
	0 0 -> 0
	0 1 -> 1
	1 0 -> 1
	1 1 -> 0
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

/* print two dimensions double array */
void printM(double a[][2], size_t rows, size_t cols) {
	for (size_t row = 0; row < rows; row++) {
		for (size_t col = 0; col < cols; col++) {
			printf("%4g", a[row][col]);
		}
		printf("\n");
	}
}


/* print int array */
void printA(int a[], size_t l) {
	printf("[ ");
	for (size_t i = 0; i < l; i++) {
		printf("%i, ", a[i]);
	}
	printf("]\n");
}

#define nInputs 2
#define nHiddenNodes 2
#define nOutputs 1
#define nTrainingSets 4

/* initWeights initialize weights of hidden neurones and of output nodes
   and initialize bias
	set weights at random values */
void initWeights(
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
	for (size_t i = 0; i < nOutputs; i++) {
		outputLayerBias[i] = getRandom();
	}
}


/* train the neural network for a number of epochs */
void train(
		double lr,
		int nEpochs,
		double hiddenLayerBias[],
		double outputLayerBias[],
		double trainingInputs[][nInputs],
		double trainingOutputs[][nOutputs],
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double hiddenLayer[],
		double outputLayer[]
		) {

	// Order of indexes of inputs in which nn is going to take the data.
	int trainingSetOrder[] = {0, 1, 2, 3};

	for (int epoch = 0; epoch < nEpochs; epoch++) {

		shuffle(trainingSetOrder, nTrainingSets); // Shuffle the order of inputs.

		for (size_t x = 0; x < nTrainingSets; x++) {
			int i = trainingSetOrder[x];

			// Forward pass.

			// Compute hidden layer activation.
			for(size_t j = 0; j < nHiddenNodes; j++) {
				double activation = hiddenLayerBias[j];
				for (size_t k = 0; k < nInputs; k++) {
					activation += trainingInputs[i][k] * hiddenWeights[k][j];
				}
				hiddenLayer[j] = sigmoid(activation); // Update hidden layer.
			}

			// Compute output layer activation.
			for (size_t j = 0; j < nOutputs; j++) {
				double activation = outputLayerBias[j];
				for (size_t k = 0; k < nHiddenNodes; k++) {
					activation += hiddenLayer[k] * outputWeights[k][j];
				}
				outputLayer[j] = sigmoid(activation); // Update output Layer.
			}

			// Print result of pass.
			printf("Input: %g %g\tOutput: %g\tExpected: %g\n",
					trainingInputs[i][0], trainingInputs[i][1],
					outputLayer[0], trainingOutputs[i][0]);

			// Backpropagation pass.

			// Compute change in output weights.
			double deltaOutput[nOutputs];
			for (size_t j = 0; j < nOutputs; j++) {
				double error = (trainingOutputs[i][j] - outputLayer[j]);
				deltaOutput[j] = error * dSigmoid(outputLayer[j]);
			}
			
			// Compute change in hidden weights.
			double deltaHidden[nHiddenNodes];
			for (size_t j = 0; j < nHiddenNodes; j++) {
				double error = 0.0f;
				for (size_t k = 0; k < nOutputs; k++) {
					error += deltaOutput[k] * outputWeights[j][k];
				}
				deltaHidden[j] = error * dSigmoid(hiddenLayer[j]);
			}
			
			// Apply change in output weights.
			for (int j = 0; j < nOutputs; j++) {
				outputLayerBias[j] += deltaOutput[j] * lr;
				for (int k = 0; k < nHiddenNodes; k++) {
					outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
				}
			}

			// Apply change in hidden weights.
			for (int j = 0; j < nHiddenNodes; j++) {
				hiddenLayerBias[j] += deltaHidden[j] * lr;
				for (int k = 0; k < nInputs; k++) {
					hiddenWeights[k][j] += trainingInputs[i][k] * deltaHidden[j] * lr;
				}
			}
		}
	}

	// Print final results after training.
	printf("\n");
	printf("====================================\n");
	printf("===|FINAL RESULTS AFTER TRAINING|===\n");
	printf("====================================\n\n");
	printf("Final Hidden Weights\n");
	for (size_t i = 0; i < nInputs; i++) {
		printf("[ ");
		for (size_t j = 0; j < nHiddenNodes; j++) {
			printf("%f ", hiddenWeights[i][j]);
		}
		printf("]\n");
	}
	printf("\n");
	printf("Final Outputs Weights\n");
	for (size_t i = 0; i < nHiddenNodes; i++) {
		printf("[ ");
		for (size_t j = 0; j < nOutputs; j++) {
			printf("%f ", outputWeights[i][j]);
		}
		printf("]\n");
	}
	printf("\n");
	printf("Final Hidden Biases\n");
	for (size_t i = 0; i < nHiddenNodes; i++) {
		printf("%f\n", hiddenLayerBias[i]);
	}
	printf("\n");
	printf("Final Output Biases\n");
	for (size_t i = 0; i < nOutputs; i++) {
		printf("%f\n", outputLayerBias[i]);
	}
}

int main() {

	double hiddenLayer[nHiddenNodes];
	double outputLayer[nOutputs];
	
	double hiddenLayerBias[nHiddenNodes];
	double outputLayerBias[nOutputs];

	double hiddenWeights[nInputs][nHiddenNodes];
	double outputWeights[nHiddenNodes][nOutputs];

	double trainingInputs[nTrainingSets][nInputs] = {{0.0f, 0.0f},
									  {0.0f, 1.0f},
									  {1.0f, 0.0f},
									  {1.0f, 1.0f}};

	double trainingOutputs[nTrainingSets][nOutputs] = {{0.0f},
									  {1.0f},
									  {1.0f},
									  {0.0f}};
	/*
	// testing shuffle algorithm
	int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	for (int i = 0; i < 50; i++) {
		shuffle(a, 10);
		printA(a, 10);
	}
	*/
	
	// Initialize weights.
	initWeights(hiddenWeights, outputWeights, outputLayerBias);

	const double lr = 0.1f; // Define learning rate of nn.
	int nEpochs = 10000;

	// Train nn.
	train(lr, nEpochs, hiddenLayerBias, outputLayerBias, trainingInputs, trainingOutputs, hiddenWeights, outputWeights, hiddenLayer, outputLayer);

	return 0;
}
