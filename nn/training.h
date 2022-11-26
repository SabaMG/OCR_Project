#pragma once

#include "network.h"

float evaluate(Layer network[], double **samples, double **results, size_t n_sample);

float train(Layer network[], double **samples, double **results, double learning_rate, size_t n_sample);

void training(Layer network[], char *training_folder_path, double learning_rate);
