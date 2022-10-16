#pragma once
#include <stdio.h>
#include <stdin.h>
#include <math.h>

double sigmoid_func(double x[], double w[], double b, size_t nb_inputs);
double grad_w(double x, double sigm, double y);
double grad_b(double sigm, double y);
double loss_func(double sigm, double y);
void collect(double layer[], size_t nb_inputs, size_t n, double w[], double* p);
void modif(size_t nb_inputs,double w[], double* p, double delta_w[], double delta_b);
void write(double layer[], size_t nb_inputs, size_t n, double w[], double b);
void learning(double layer[], size_t nb_inputs, size_t nb_neurons, double test_in[], double test_out[], size_t nb_test, size_t epochs);
