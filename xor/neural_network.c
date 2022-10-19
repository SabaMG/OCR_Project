#include <stdio.h>
#include <math.h>
#include "utils.h"

/* forward pass
   outputLayer array is updated after pass */
void forward_pass(
		double inputs[],
		double hiddenLayerBias[],
		double outputLayerBias[],
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double hiddenLayer[],
		double outputLayer[]
		) {

	// Compute hidden layer activation.
	for (size_t j = 0; j < nHiddenNodes; j++)
	{
		double activation = hiddenLayerBias[j];
		for (size_t k = 0; k < nInputs; k++)
		{
			activation += inputs[k] * hiddenWeights[k][j];
		}
		hiddenLayer[j] = sigmoid(activation); // Update hidden layer.
	}

	// Compute output layer activation.
	for (size_t j = 0; j < nOutputs; j++)
	{
		double activation = outputLayerBias[j];
		for (size_t k = 0; k < nHiddenNodes; k++)
		{
			activation += hiddenLayer[k] * outputWeights[k][j];
		}
		outputLayer[j] = sigmoid(activation); // Update output Layer.
	}
}

/* backpropagation
   update weights and bias after forward pass */
void backpropagation_pass(
		int i,
		double lr,
		double hiddenLayerBias[],
		double outputLayerBias[],
		double trainingInputs[][nInputs],
		double trainingOutputs[][nOutputs],
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double hiddenLayer[],
		double outputLayer[]
		) {

	double deltaOutput[nOutputs];
	for (size_t j = 0; j < nOutputs; j++)
	{
		double error = (trainingOutputs[i][j] - outputLayer[j]);
		deltaOutput[j] = error * dSigmoid(outputLayer[j]);
	}

	// Compute change in hidden weights.
	double deltaHidden[nHiddenNodes];
	for (size_t j = 0; j < nHiddenNodes; j++)
	{
		double error = 0.0f;
		for (size_t k = 0; k < nOutputs; k++)
		{
			error += deltaOutput[k] * outputWeights[j][k];
		}
		deltaHidden[j] = error * dSigmoid(hiddenLayer[j]);
	}

	// Apply change in output weights.
	for (int j = 0; j < nOutputs; j++)
	{
		outputLayerBias[j] += deltaOutput[j] * lr;
		for (int k = 0; k < nHiddenNodes; k++)
		{
			outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
		}
	}

	// Apply change in hidden weights.
	for (int j = 0; j < nHiddenNodes; j++)
	{
		hiddenLayerBias[j] += deltaHidden[j] * lr;
		for (int k = 0; k < nInputs; k++)
		{
			hiddenWeights[k][j] += trainingInputs[i][k] * deltaHidden[j] * lr;
		}
	}
}

/* train the neural network for a number of epochs */
void train(
		int vflag,
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

	/* order of indexes of inputs in which nn is going to take the data */
	int trainingSetOrder[] = {0, 1, 2, 3};

	for (int epoch = 0; epoch < nEpochs; epoch++)
	{

		shuffle(trainingSetOrder, nTrainingSets); /* shuffle training set for better performances */

		for (size_t x = 0; x < nTrainingSets; x++) /* do forward + backward pass for each 'case' */
		{
			int i = trainingSetOrder[x];

			/* forward pass */
			forward_pass(trainingInputs[i], hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, hiddenLayer, outputLayer);

			if (vflag) /* print result of pass */
			{
				printf("%i - %i Input: %g %g (%i)\tOutput: %5f\tExpected: %g\n",
						epoch, (int)x,
						trainingInputs[i][0], trainingInputs[i][1], i,
						outputLayer[0], trainingOutputs[i][0]);
			}

			/* backpropagation pass */
			backpropagation_pass(i, lr, hiddenLayerBias, outputLayerBias, trainingInputs, trainingOutputs, hiddenWeights, outputWeights, hiddenLayer, outputLayer);
		}
	}

	/* print final results after training */
	printf("====================================\n");
	printf("===|FINAL RESULTS AFTER TRAINING|===\n");
	printf("====================================\n\n");
	printf("Final Hidden Weights\n");
	for (size_t i = 0; i < nInputs; i++)
	{
		printf("[ ");
		for (size_t j = 0; j < nHiddenNodes; j++)
		{
			printf("%f ", hiddenWeights[i][j]);
		}
		printf("]\n");
	}
	printf("\nFinal Outputs Weights\n");
	for (size_t i = 0; i < nHiddenNodes; i++)
	{
		printf("[ ");
		for (size_t j = 0; j < nOutputs; j++)
		{
			printf("%f ", outputWeights[i][j]);
		}
		printf("]\n");
	}
	printf("\nFinal Hidden Biases\n");
	for (size_t i = 0; i < nHiddenNodes; i++)
	{
		printf("%f\n", hiddenLayerBias[i]);
	}
	printf("\nFinal Output Biases\n");
	for (size_t i = 0; i < nOutputs; i++)
	{
		printf("%f\n", outputLayerBias[i]);
	}

	/* do one last forward pass */
	printf("\nLast Training Session\n");
	for (size_t x = 0; x < nTrainingSets; x++)
	{
		forward_pass(trainingInputs[x], hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, hiddenLayer, outputLayer);

		double percentage = round(outputLayer[0]) == 0.0f ? (1.0f - outputLayer[0]) * 100.0f : outputLayer[0] * 100.0f;

		printf("Input: %g %g\tOutput: %5f\tExpected: %g\t %5f%%\n",
				trainingInputs[x][0], trainingInputs[x][1],
				outputLayer[0], trainingOutputs[x][0], percentage);
	}
}
