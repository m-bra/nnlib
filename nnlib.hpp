#ifndef NNLIB_HPP_INCLUDED
#define NNLIB_HPP_INCLUDED

typedef float nn_real;

struct nn_neuron
{
    nn_real out;

    nn_real treshold;
    unsigned in_neurons_cnt;
    nn_real *in_neurons_weights;
    nn_neuron **in_neurons;

    void init(unsigned in_neurons_cnt);
    void free();
    void calc_out();
};

struct nn_layer
{
    unsigned neurons_cnt;
    nn_layer *base;
    nn_neuron *neurons;

    void init(unsigned neuron_cnt, nn_layer *base);
    void free();
    void calc_out();
};

struct nn_network
{
    unsigned layers_cnt;
    nn_layer *layers;

    void init(unsigned layers_cnt, unsigned const *neurons_cnt);
    void free();
    void copy_from(nn_network const *src);

    void setinputs(nn_real const *inputs);
    void getoutputs(nn_real *outputs);
    void calc_out();
    void fromcode(struct nn_network_code *code);
    void dump();
};

struct nn_network_code
{
    unsigned values_cnt;
    nn_real *values;

    void init(nn_network *);
    void free();
};

#endif/*NNLIB_HPP_INCLUDED*/
