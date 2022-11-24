#pragma once

#include "network.h"

float evaluate(Layer network[], double **samples, double **results, size_t n_sample);

float train(Layer network[], double **samples, double **results, double learning_rate, size_t n_sample);
