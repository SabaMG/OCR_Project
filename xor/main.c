/*
	Author: Charles-Antoine Leger

	Mini neural network capable of learning the xor (or exclusive) function
	
	Weights and biases of neurons can be saved and loaded from a specific file
	The sigmoid function is used for the activation function of neurons.
	The derivative of the sigmoid function is used for the cost function.

	XOR:
	inputs -> output
	   0 0 -> 0
	   0 1 -> 1
	   1 0 -> 1
	   1 1 -> 0
		
	TODO
	- diviser en plusieurs fichier: main.c, train.c, utils.c
	- ajouter la sauvegarde / chargement des poids depuis un fichier.
	- ajouter un prompt pour saisir les inputs.
	- ajouter un print pour le resultat.
	- ajouter un timer pour l'apprentissage.
	- ajouter les options
		-ep (nombre d'epochs, par default: 10000)
		-v (verbose: afficher les logs de l'apprentissage)
		-lr (learning rate, par default: 0.1)
		-lw (load weights, suivi d'un nom de fichier)
		-sw (save weights, suivi d'un nom de fichier)

*/

#include <stdio.h>
#include "train.h"
#include "utils.h"

/* print two dimensions double array */
/*
void printM(double a[][2], size_t rows, size_t cols) {
	for (size_t row = 0; row < rows; row++) {
		for (size_t col = 0; col < cols; col++) {
			printf("%4g", a[row][col]);
		}
		printf("\n");
	}
}
*/


/* print int array */
/*
void printA(int a[], size_t l) {
	printf("[ ");
	for (size_t i = 0; i < l; i++) {
		printf("%i, ", a[i]);
	}
	printf("]\n");
}
*/

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
