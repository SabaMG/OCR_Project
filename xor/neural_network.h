#pragma once
#include "impl.h"
extern const int nInputs;
extern const int nHiddenNodes;
extern const int nOutputs;
extern const int nTrainingSets;

void forward_pass(
		double inputs[],
		Layer aLayers[],
		double hiddenLayer[],
		double outputLayer[]
);

void backpropagation_pass(
		int i,
		double lr,
		double trainingInputs[][nInputs],
		double trainingOutputs[][nOutputs],
		Layer aLayers[],
		double hiddenLayer[],
		double outputLayer[]
		);

void train(
		int vflag,
		double lr,
		int nEpochs,
		double trainingInputs[][nInputs],
		double trainingOutputs[][nOutputs],
		Layer aLayers[],
		double hiddenLayer[],
		double outputLayer[]
		);
