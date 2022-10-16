#pragma once
#define nInputs 2
#define nHiddenNodes 2
#define nOutputs 1
#define nTrainingSets 4

double sigmoid(double x);
double dSigmoid(double x);
double getRandom();
void shuffle(int a[], size_t n);
int initWeights(
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double outputLayerBias[]
		);


