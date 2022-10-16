#pragma once
extern const int nInputs;
extern const int nHiddenNodes;
extern const int nOutputs;
extern const int nTrainingSets;

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
		);
