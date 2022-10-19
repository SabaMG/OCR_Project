#pragma once
extern const int nInputs;
extern const int nHiddenNodes;
extern const int nOutputs;
extern const int nTrainingSets;

int forward_pass(
		double inputs[],
		double hiddenLayerBias[],
		double outputLayerBias[],
		double hiddenWeights[][nHiddenNodes],
		double outputWeights[][nOutputs],
		double hiddenLayer[],
		double outputLayer[]
);
int train(
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
		);
