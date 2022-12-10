#pragma once

#include "network.h"

int evaluate(int ordre[], double **samples, double **results, int n_sample, int v_flag);

int train(int ordre[], Layer network[], double **samples, double **results, double learning_rate, size_t total, int v_flag, int n_sample);

void training(Layer network[], char *training_folder_path, double learning_rate, int v_flag, int w_flag);

void ocr(Layer network[], char *folder_path, int w_flag, int debug);
