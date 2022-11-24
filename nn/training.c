#include <math.h>

#include "ocr.h"
#include "network.h"

// Evaluate network
float evaluate(Layer network[], double **samples, double **results, size_t n_sample) {
	float error = 0;
	unsigned last = network->size - 1;
	for (size_t i = 0; i < n_sample; i++)
	{
		float dist = 0;
		feed_forward(network, samples[i]);
		for (unsigned j = 0; j < network[last].size; j++)
			dist += pow(network[last].neurons[j].value - results[i][j], 2);
		error += sqrt(dist);
	}
	return error / (n_sample * 2);
}

// Train network
float train(Layer network[], double **samples, double **results, double learning_rate, size_t n_sample) {
	//shuffleSample(samples, results, n_sample);
	//sizeBatch = sizeBatch < n_sample ? sizeBatch : n_sample;
	for (int i = 0; i < 2000; i++)
		//for (size_t j = 0; j < sizeBatch; j++)
		for (size_t j = 0; j < n_sample; j++)
		{
			feed_forward(network, samples[j]);
			backpropagation(network, results[j]);
			update_network(network, learning_rate);
		}
	return evaluate(network, samples, results, n_sample);
}
