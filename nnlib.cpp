#include "nnlib.hpp"
#include "talloc.hpp"
#include <cstdlib>
#include <ctgmath>
#include <iostream>
#include <cassert>

void nn_neuron::init(unsigned a_in_neurons_cnt)
{
	out = 0;
	treshold = 0;
	in_neurons_cnt = a_in_neurons_cnt;
	if (in_neurons_cnt)
	{
		talloc(in_neurons_weights, in_neurons_cnt);
		talloc(in_neurons, in_neurons_cnt);
	}
	else
	{
		in_neurons_weights = 0;
		in_neurons = 0;
	}
}

void nn_neuron::free()
{
	if (in_neurons_cnt)
	{
		tfree(in_neurons_weights);
		tfree(in_neurons);
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

void nn_neuron::calc_out()
{
	if (in_neurons_cnt)
	{
		nn_real activation = 0;
		for (int i = 0; i < in_neurons_cnt; ++i)
		{
			activation+= in_neurons[i]->out * in_neurons_weights[i];
		}
		activation-= treshold;

		// smooth the output
		//out = sigmoid(activation);
		out = activation < 0 ? 0 : 1;
	}
}

void nn_layer::init(unsigned a_neuron_cnt, nn_layer *abase)
{
	neurons_cnt = a_neuron_cnt;
	talloc(neurons, neurons_cnt);
	base = abase;

	for (int i = 0; i < neurons_cnt; ++i)
	{
		if (base)
		{
			neurons[i].init(base->neurons_cnt);
			neurons[i].treshold = (rand() % 400) / 100. - 2;
			for (int j = 0; j < base->neurons_cnt; ++j)
			{
				neurons[i].in_neurons_weights[j] = (rand() % 400) / 100. - 2;
				neurons[i].in_neurons[j] = &base->neurons[j];
			}
		}
		else
			neurons[i].init(0);
	}
}

void nn_layer::free()
{
	for (int i = 0; i < neurons_cnt; ++i)
	{
		neurons[i].free();
	}
	tfree(neurons);
}

void nn_layer::calc_out()
{
	for (int i = 0; i < neurons_cnt; ++i)
		neurons[i].calc_out();
}

void nn_network::init(unsigned a_layers_cnt, unsigned *a_neurons_cnt)
{
	layers_cnt = a_layers_cnt;
	talloc(layers, layers_cnt);
	for (int l = 0; l < layers_cnt; ++l)
		if (l)
			layers[l].init(a_neurons_cnt[l], &layers[l - 1]);
		else
			layers[l].init(a_neurons_cnt[l], 0);
}

void nn_network::free()
{
	for (int l = 0; l < layers_cnt; ++l)
		layers[l].free();
	tfree(layers);
}

void nn_network::setinputs(nn_real *inputs)
{
	for (int n = 0; n < layers[0].neurons_cnt; ++n)
		layers[0].neurons[n].out = inputs[n];
}

void nn_network::getoutputs(nn_real *outputs)
{
	int const last_layer = layers_cnt - 1;
	for (int n = 0; n < layers[last_layer].neurons_cnt; ++n)
		outputs[n] = layers[last_layer].neurons[n].out;
}

void nn_network::calc_out()
{
	for (int l = 0; l < layers_cnt; ++l)
		if (l)
			layers[l].calc_out();
}

void nn_network::dump()
{
	std::cout << "nn_network_dump (\n";
	for (int l = 0; l < layers_cnt; ++l)
	{
		std::cout << "  layer " << l << " (\n";
		for (int n = 0; n < layers[l].neurons_cnt; ++n)
		{
			std::cout << "    neuron " << n << " (";
			std::cout << "o" << layers[l].neurons[n].out;
			if (l > 0)
			{
				std::cout << " t" << layers[l].neurons[n].treshold;
				for (int w = 0; w < layers[l - 1].neurons_cnt; ++w)
					std::cout << " w" <<
						layers[l].neurons[n].in_neurons_weights[w];
			}
			std::cout << ")\n";
		}
		std::cout << ")\n";
	}
	std::cout << ")\n";
}

void nn_network::fromcode(nn_network_code *code)
{
	unsigned b = 0;
	for (int l = 1; l < layers_cnt; ++l)
		for (int n = 0; n < layers[l].neurons_cnt; ++n)
		{
			layers[l].neurons[n].treshold = code->values[b++];
			for (int w = 0; w < layers[l-1].neurons_cnt; ++w)
				layers[l].neurons[n].in_neurons_weights[w] = code->values[b++];
		}
}

void nn_network_code::init(nn_network *network)
{
	values_cnt = 0;
	for (int l = 1; l < network->layers_cnt; ++l)
		values_cnt+= network->layers[l].neurons_cnt * (1 + network->layers[l - 1].neurons_cnt);

	talloc(values, values_cnt);
	unsigned b = 0;
	for (int l = 1; l < network->layers_cnt; ++l)
		for (int n = 0; n < network->layers[l].neurons_cnt; ++n)
		{
			values[b++] = network->layers[l].neurons[n].treshold;
			for (int w = 0; w < network->layers[l-1].neurons_cnt; ++w)
				values[b++] = network->layers[l].neurons[n].in_neurons_weights[w];
		}
	if (b != values_cnt)
	{
		std::cerr << "ERROR: NNLIB: Internal error (" << __FILE__ << "@" << __LINE__ << ")\n";
		exit(EXIT_FAILURE);
	}
}

void nn_network_code::free()
{
	tfree(values);
}
