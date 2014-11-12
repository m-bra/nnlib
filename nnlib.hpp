
typedef float nn_real;
typedef struct nn_network nn_network;
typedef struct nn_network_code nn_network_code;

void nn_network_init(nn_network *network, unsigned layers_cnt, unsigned *neurons_cnt);
void nn_network_free(nn_network *network);
void nn_network_setinputs(nn_network *network, nn_real *inputs);
void nn_network_getoutputs(nn_network *network, nn_real *outputs);
void nn_network_calc_out(nn_network *network);
void nn_network_fromcode(nn_network *network, nn_network_code *code);

void nn_network_code_init(nn_network_code *code, nn_network *network);
void nn_network_code_free(nn_network_code *code);
