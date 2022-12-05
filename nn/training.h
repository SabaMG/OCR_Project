#pragma once

#include "network.h"

float evaluate(Layer network[], double **samples, double **results, size_t n_sample, int v_flag);

float train(Layer network[], double **samples, double **results, double learning_rate, size_t n_sample, int v_flag);

void training(Layer network[], char *training_folder_path, double learning_rate, int v_flag);
