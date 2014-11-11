#include "nnlib.h"
#include <talloc.hpp>

typedef nn_real float;

typedef struct nn_neuron
{
	nn_real activation;
	unsigned in_neurons_cnt;
	nn_real *in_neurons_weights;
} nn_neuron;

void nn_neuron_init_min(nn_neuron *n, unsigned in_neurons_cnt)
{
	n->in_neurons_cnt = in_neurons_cnt;
	n->in_neurons_weights = talloc(in_neurons_cnt);
}

void nn_neuron_init(nn_neuron *n, nn_real activation, unsigned in_neurons_cnt, nn_real *in_neurons_weights)
{
	n->activation = activation;
	n->in_neurons_cnt = in_neurons_cnt;
	n->in_neurons_weights = talloc(in_neurons_cnt);
	tmemcpy(n->in_neurons_weights, in_neurons_weights, in_neurons_cnt);
}

void nn_neuron_free(nn_neuron *n)
{
	tfree(n->in_neuron_weights);
}

nn_real nn_neuron_calc_out(nn_neuron *n, nn_neuron *in_neurons)
{
	
}

struct nn_layer
{
	unsigned neurons_cnt;
	nn_neuron *neurons;
	
};
