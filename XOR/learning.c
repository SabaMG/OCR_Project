#include "learning.h"

// This function calculates the sigmoid function
// return (double) : the res of sigmoid function
double sigmoid_func(double x[], double w[], double b, size_t nb_inputs)
{
    double z = 0;
    for (size_t i = 0; i < nb_inputs; ++i)
    {
	z += x[i] * w[i];
    }
    z += b;
    return 1/(1 + exp(-z));
}

// This function calculates grad of weight in function of
// the input x, the output calculated sigm and the output y we
// want to have
// return (double) : the res of drag de w
double grad_w(double x, double sigm, double y)
{
    return ((sigm - y) * sigm ) * (1 - sigm) * x;
}

// This function calculates grad of bia in function of
// the output calculated sigm and the output y we
// want to have
// return (double) : the res of drag de b
double grad_b(double sigm, double y)
{
    return ((sigm - y) * sigm ) * (1 - sigm);
}

// This function calculates loss in function of
// the output calculated sigm and the output y we
// want to have
// return (double) : the res of loss function
double loss_func(double sigm, double y)
{
    return pow(y - sigm, 2);
}

// This function collect weights and bia of neuron n
// store in layer array
void collect(double layer[], size_t nb_inputs, size_t n, double w[], double* p)
{
    // collect weights
    for(size_t j = 0; j < nb_inputs; ++j)
    {
	w[j] = layer[(n * (nb_inputs + 1)) + j];
    }

    // collect bias
    *p = layer[n * (nb_inputs + 1) + nb_inputs];
}

// This function modify weights and bia with delta_w and delta_b
// values obtained after learning
void modif(size_t nb_inputs,double w[], double* p, double delta_w[], double delta_b)
{
    
}

// This function write the modification of weights and bia
// of neuron n in layer array
void write(double layer[], size_t nb_inputs, size_t n, double w[], double b)
{

}

// This function train the layer
void learning(double layer[], size_t nb_inputs, size_t nb_neurons, double test_in[], double test_out[], size_t nb_test, size_t epochs)
{
    // number of epochs to learn
    for (size_t i = 0; i < epochs; ++i)
    {
	// foreach neurons
	for(size_t n = 0; n < nb_neurons; ++n)
	{
	    // collect weights and bia of n
	    double w[nb_inputs];
	    double b;
	    collect(layer, nb_inputs, n, w, &b);

	    // init delta_w, delta_b and loss function;
	    double delta_w[nb_inputs] = {};
	    double delta_b = 0;
	    double loss = 0;

	    // train the current neuron with test_in
	    for(size_t t = 0; t < nb_test; ++t)
	    {
		// collect test inputs
		double x[nb_input];
		for(size_t j = 0; j < nb_inputs; ++j)
		{
		    x[j] = test_in[t * nb_inputs + j];
		}
		
		// sigmoid function
		double sigm = sigmoid_func(x, w, b, nb_input);
		
		// Calcul the grad of weights and bias and loss function
		for(size_t j = 0; j < nb_inputs; ++j)
		{
		    delta_w[j] += grad_w(x[j], sigm, test_out[t]);
		}
		delta_b += grad_b(sigm, test_out[t]);
		loss += loss_func(sigm, test_out[t]);
	    }
	    
	    // make changes
	    change(nb_inputs, w, &b, delta_w, delta_b);
	    // write weights and bia modification
	    write(...)

	    printf("current loss : %f", loss);
	}
    }
}
