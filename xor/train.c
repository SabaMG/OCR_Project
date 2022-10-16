#include <stdio.h>
#include "utils.h"

/* train the neural network for a number of epochs */
int train(
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

	return 0;
}

