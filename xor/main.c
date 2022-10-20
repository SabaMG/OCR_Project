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
	- ajouter la sauvegarde / chargement des poids depuis un fichier.
	- ajouter les options
		-l (load weights, suivi d'un nom de fichier)
		-s (save weights, suivi d'un nom de fichier)

*/

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "neural_network.h"
#include "utils.h"

int main(int argc, char **argv) {

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

	int vflag = 0; /* verbose mode */
	int tflag = 0; /* trainig mode */
	char *evalue = NULL; /* set epochs for training */
	char *rvalue = NULL; /* set learning rate for training */
	char *lvalue = NULL; /* set load option (path) */
	char *arg;

	/* parsing command line arguments */
	int i = 1;
	while (i < argc) {
		arg = argv[i];
		if (strcmp(arg, "-v") == 0)
			vflag = 1;
		else if (strcmp(arg, "-t") == 0) {
			tflag = 1;
		}
		else if (strcmp(arg, "-e") == 0) {
			if (!tflag)
				errx(EXIT_FAILURE, "Option -e requires to be in training mode (add -t).");
			i++;
			if (i < argc)
				evalue = argv[i];
			else
				errx(EXIT_FAILURE, "Option -e requires an argument.");
		}
		else if (strcmp(arg, "-r") == 0) {
			if (!tflag)
				errx(EXIT_FAILURE, "Option -r requires to be in training mode (add -t).");
			i++;
			if (i < argc)
				rvalue = argv[i];
			else
				errx(EXIT_FAILURE, "Option -r requires an argument.");
		}
		else if (strcmp(arg, "-l") == 0) {
			if (tflag)
				errx(EXIT_FAILURE, "Option -l requires to be in normal mode (remove -t).");
			i++;
			if (i < argc)
				lvalue = argv[i];
			else
				errx(EXIT_FAILURE, "Option -l requires an argument.");
		}
		else
			errx(EXIT_FAILURE, "Unknown option %s.", argv[i]);
		i++;
	}

	if (lvalue != NULL) { /* load_weights from lvalue */
		int error = load_weights(lvalue);
		if (error)
			errx(1, "Failed to load weights.\n");
		return 0;
	}

	if (!tflag) { /* if normal mode (not in training mode) */
		/* !!! BECAUSE SAVE / LOAD WEIGHTS NOT IMPLEMENTED !!! */
		const double defaultLR = 0.1f; /* learning rate */
		const int defaultNumEpochs = 10000; /* default epochs value */
		initWeights(hiddenWeights, outputWeights, outputLayerBias);
		train(vflag, defaultLR, defaultNumEpochs, hiddenLayerBias, outputLayerBias, trainingInputs, trainingOutputs, hiddenWeights, outputWeights, hiddenLayer, outputLayer);
		printf("\n");
		printf("END OF TRAINING\n");
		printf("\n");
		/* !!! BECAUSE SAVE / LOAD WEIGHTS NOT IMPLEMENTED !!! */

		char i1;
		while (1) { /* get first input */
			printf("Enter first input: ");
			i1 = getchar();
			if (i1 == '0' || i1 == '1') {
				break;
			}
			else {
				printf("Input should be 0 or 1, try again.\n");
			}
		}
		/* without it bugs! */ getchar(); /* without its bugs! */
		char i2;
		while (1) { /* get second input */
			printf("Enter second input: ");
			i2 = getchar();
			if (i2 == '0' || i2 == '1') {
				break;
			}
			else {
				printf("Input should be 0 or 1, try again.\n");
			}
		}

		/* build input array */
		double inputs[2];
		inputs[0] = (double)(i1 - 48);
		inputs[1] = (double)(i2 - 48);

		/* do forward pass */
		forward_pass(inputs, hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, hiddenLayer, outputLayer);

		/* print results */
		//system("clear"); /* clear screen for print results */
		printf("\n");
		printf("RESULTS AFTER FORWARD PASS IN THE NEURAL NETWORK\n");
		printf("Input: %g %g\tOutput: %.f\t\n",
				inputs[0], inputs[1], outputLayer[0]);
	}

	if (tflag) { /* if training mode */
		const double defaultLR = 0.1f; /* learning rate */
		const int defaultNumEpochs = 10000; /* default epochs value */

		/* change epochs if in args */
		int nEpochs = defaultNumEpochs;
		if (evalue != NULL) {
			nEpochs = atoi(evalue);
		}
		/* change lr if in arg */
		double lr = defaultLR;
		if (rvalue != NULL) {
			lr = atof(rvalue);
		}

		/* initialization of weights */
		initWeights(hiddenWeights, outputWeights, outputLayerBias);

		double time_spent = 0.0f; /* start timer */
		clock_t begin = clock();

		/* do train */
		train(vflag, lr, nEpochs, hiddenLayerBias, outputLayerBias, trainingInputs, trainingOutputs, hiddenWeights, outputWeights, hiddenLayer, outputLayer);

		clock_t end = clock(); /* stop timer */
		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

		/* print some info */
		printf("\nTraining successfully completed in %f seconds (epochs = %i; learning rate = %g)\n", time_spent, nEpochs, lr);
	}

	/*
	// testing shuffle algorithm
	int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	for (int i = 0; i < 50; i++) {
	shuffle(a, 10);
	printA(a, 10);
	}
	 */

	return EXIT_SUCCESS;
}
