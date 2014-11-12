#include "nnlib.hpp"
#include "talloc.hpp"
#include <cstdlib>
#include <ctgmath>
#include <iostream>
#include <cassert>

typedef struct nn_neuron
{
	nn_real out;
	bool dirty;
	
	nn_real treshold;
	unsigned in_neurons_cnt;
	nn_real *in_neurons_weights;
	nn_neuron **in_neurons;
} nn_neuron;

void nn_neuron_init(nn_neuron *n, unsigned in_neurons_cnt)
{
	n->out = 0;
	n->dirty = true;
	n->treshold = 0;
	n->in_neurons_cnt = in_neurons_cnt;
	if (in_neurons_cnt)
	{
		talloc(n->in_neurons_weights, in_neurons_cnt);
		talloc(n->in_neurons, in_neurons_cnt);
	}
	else
	{
		n->in_neurons_weights = 0;
		n->in_neurons = 0;
	}
}

void nn_neuron_free(nn_neuron *n)
{
	if (n->in_neurons_cnt)
	{
		tfree(n->in_neurons_weights);
		tfree(n->in_neurons);
	}
}

// the sigmoid function, where p is the inverse steppiness
// the lower p is, the steppier the function result
template <typename T>
T sigmoid(T x, T p = 1)
{
	T const e = 2.71828182845904523536;
	return 1 / (1 + pow(e, -x/p));
}

bool nn_neuron_calc_out(nn_neuron *n)
{
	if (n->in_neurons_cnt)
	{
		nn_real activation = 0;
		for (int i = 0; i < n->in_neurons_cnt; ++i)
		{
			activation+= n->in_neurons[i]->out * n->in_neurons_weights[i];
		}
		activation-= n->treshold;
	
		// smooth the output
		n->out = sigmoid(activation);
	}
}

typedef struct nn_layer
{
	unsigned neurons_cnt;
	nn_layer *base;
	nn_neuron *neurons;
} nn_layer;

void nn_layer_init(nn_layer *l, unsigned neuron_cnt, nn_layer *base)
{
	l->neurons_cnt = neuron_cnt;
	talloc(l->neurons, neuron_cnt);
	l->base = base;
	
	for (int i = 0; i < neuron_cnt; ++i)
	{
		if (base)
		{
			nn_neuron_init(&l->neurons[i], base->neurons_cnt);
			for (int j = 0; j < l->base->neurons_cnt; ++j)
			{
				l->neurons[i].in_neurons_weights[j] = (rand() % 200) / 100.;
				l->neurons[i].in_neurons[j] = &base->neurons[j];
			}
		}
		else
			nn_neuron_init(&l->neurons[i], 0);
	}
}

void nn_layer_free(nn_layer *l)
{
	for (int i = 0; i < l->neurons_cnt; ++i)
	{
		nn_neuron_free(&l->neurons[i]);
	}
	tfree(l->neurons);
}

void nn_layer_neurons_calc_out(nn_layer *l)
{
	for (int i = 0; i < l->neurons_cnt; ++i)
		nn_neuron_calc_out(&l->neurons[i]);
}

typedef struct nn_network
{
	unsigned layers_cnt;
	nn_layer *layers;
} nn_network;

void nn_network_init(nn_network *network, unsigned layers_cnt, unsigned *neurons_cnt)
{
	network->layers_cnt = layers_cnt;
	talloc(network->layers, layers_cnt);
	for (int l = 0; l < network->layers_cnt; ++l)
		if (l)
			nn_layer_init(&network->layers[l], neurons_cnt[l], &network->layers[l - 1]);
		else
			nn_layer_init(&network->layers[l], neurons_cnt[l], 0);
}

void nn_network_free(nn_network *network)
{
	for (int l = 0; l < network->layers_cnt; ++l)
		nn_layer_free(&network->layers[l]);
	tfree(network->layers);
}

void nn_network_setinputs(nn_network *network, nn_real *inputs)
{
	for (int n = 0; n < network->layers[0].neurons_cnt; ++n)
		network->layers[0].neurons[n].out = inputs[n];
}

void nn_network_getoutputs(nn_network *network, nn_real *outputs)
{
	int const last_layer = network->layers_cnt - 1;
	for (int n = 0; n < network->layers[last_layer].neurons_cnt; ++n)
		outputs[n] = network->layers[last_layer].neurons[n].out;		
}

void nn_network_calc_out(nn_network *network)
{
	for (int l = 0; l < network->layers_cnt; ++l)
		if (l)
			nn_layer_neurons_calc_out(&network->layers[l]);
}

typedef struct nn_network_code
{
	unsigned values_cnt;
	nn_real *values;
} nn_network_code;

void nn_network_fromcode(nn_network *network, nn_network_code *code)
{
	unsigned b = 0;
	for (int l = 1; l < network->layers_cnt; ++l)
		for (int n = 0; n < network->layers[l].neurons_cnt; ++n)
		{
			network->layers[l].neurons[n].treshold = code->values[b++];
			for (int w = 0; w < network->layers[l-1].neurons_cnt; ++w)
				network->layers[l].neurons[n].in_neurons_weights[w] = code->values[b++];
		}
}

void nn_network_code_init(nn_network_code *code, nn_network *network)
{
	code->values_cnt = 0;
	for (int l = 1; l < network->layers_cnt; ++l)
		code->values_cnt+= network->layers[l].neurons_cnt * (1 + network->layers[l - 1].neurons_cnt);
		
	talloc(code->values, code->values_cnt);
	unsigned b = 0;
	for (int l = 1; l < network->layers_cnt; ++l)
		for (int n = 0; n < network->layers[l].neurons_cnt; ++n)
		{
			code->values[b++] = network->layers[l].neurons[n].treshold;
			for (int w = 0; w < network->layers[l-1].neurons_cnt; ++w)
				code->values[b++] = network->layers[l].neurons[n].in_neurons_weights[w];
		}
	if (b != code->values_cnt)
	{
		std::cerr << "ERROR: NNLIB: Internal error (" << __FILE__ << "@" << __LINE__ << ")\n";
		exit(EXIT_FAILURE);
	}
}

void nn_network_code_free(nn_network_code *code)
{
	tfree(code->values);
}
