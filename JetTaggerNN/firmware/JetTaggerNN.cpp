#include <iostream>

#include "JetTaggerNN.h"
#include "parameters.h"

void JetTaggerNN(
    input_t inputs[N_INPUT_1_1*N_INPUT_2_1],
    layer22_t layer22_out[N_LAYER_22], layer24_t layer24_out[N_LAYER_20]
) {

    // hls-fpga-machine-learning insert IO
    #pragma HLS INLINE 

#ifndef __SYNTHESIS__
    static bool loaded_weights = false;
    if (!loaded_weights) {
        // hls-fpga-machine-learning insert load weights
        nnet::load_weights_from_txt<batchnorm_inputs_scale_t, 21>(s2, "s2.txt");
        nnet::load_weights_from_txt<batchnorm_inputs_bias_t, 21>(b2, "b2.txt");
        nnet::load_weights_from_txt<qdense_phi_1_weight_t, 672>(w26, "w26.txt");
        nnet::load_weights_from_txt<qdense_phi_1_bias_t, 32>(b26, "b26.txt");
        nnet::load_weights_from_txt<qdense_phi_2_weight_t, 1024>(w27, "w27.txt");
        nnet::load_weights_from_txt<qdense_phi_2_bias_t, 32>(b27, "b27.txt");
        nnet::load_weights_from_txt<weight11_t, 1024>(w11, "w11.txt");
        nnet::load_weights_from_txt<bias11_t, 32>(b11, "b11.txt");
        nnet::load_weights_from_txt<weight14_t, 1024>(w14, "w14.txt");
        nnet::load_weights_from_txt<bias14_t, 32>(b14, "b14.txt");
        nnet::load_weights_from_txt<weight16_t, 1024>(w16, "w16.txt");
        nnet::load_weights_from_txt<bias16_t, 32>(b16, "b16.txt");
        nnet::load_weights_from_txt<weight20_t, 256>(w20, "w20.txt");
        nnet::load_weights_from_txt<bias20_t, 8>(b20, "b20.txt");
        nnet::load_weights_from_txt<weight22_t, 32>(w22, "w22.txt");
        nnet::load_weights_from_txt<bias22_t, 1>(b22, "b22.txt");
        loaded_weights = true;
    }
#endif

    // ****************************************
    // NETWORK INSTANTIATION
    // ****************************************

    // hls-fpga-machine-learning insert layers

    layer2_t layer2_out[N_INPUT_1_1*N_INPUT_2_1];
    #pragma HLS ARRAY_PARTITION variable=layer2_out complete dim=0
    nnet::normalize<input_t, layer2_t, config2>(inputs, layer2_out, s2, b2); // batchnorm_inputs

    layer26_t layer26_out[N_OUTPUTS_26*N_FILT_26];
    #pragma HLS ARRAY_PARTITION variable=layer26_out complete dim=0
    nnet::pointwise_conv_1d_cl<layer2_t, layer26_t, config26>(layer2_out, layer26_out, w26, b26); // qDense_phi_1

    layer5_t layer5_out[N_LAYER_1_3*N_LAYER_2_3];
    #pragma HLS ARRAY_PARTITION variable=layer5_out complete dim=0
    nnet::relu<layer26_t, layer5_t, relu_config5>(layer26_out, layer5_out); // qActivation_phi_1

    layer27_t layer27_out[N_OUTPUTS_27*N_FILT_27];
    #pragma HLS ARRAY_PARTITION variable=layer27_out complete dim=0
    nnet::pointwise_conv_1d_cl<layer5_t, layer27_t, config27>(layer5_out, layer27_out, w27, b27); // qDense_phi_2

    layer8_t layer8_out[N_LAYER_1_6*N_LAYER_2_6];
    #pragma HLS ARRAY_PARTITION variable=layer8_out complete dim=0
    nnet::relu<layer27_t, layer8_t, relu_config8>(layer27_out, layer8_out); // qActivation_phi_2

    layer9_t layer9_out[N_LAYER_1_6*N_LAYER_2_6];
    #pragma HLS ARRAY_PARTITION variable=layer9_out complete dim=0
    nnet::linear<layer8_t, layer9_t, linear_config9>(layer8_out, layer9_out); // qActivationForPool

    layer10_t layer10_out[N_FILT_10];
    #pragma HLS ARRAY_PARTITION variable=layer10_out complete dim=0
    nnet::global_pooling1d_cl<layer9_t, layer10_t, config10>(layer9_out, layer10_out); // avgpool

    layer11_t layer11_out[N_LAYER_11];
    #pragma HLS ARRAY_PARTITION variable=layer11_out complete dim=0
    nnet::dense<layer10_t, layer11_t, config11>(layer10_out, layer11_out, w11, b11); // qDense_rho_1

    layer13_t layer13_out[N_LAYER_11];
    #pragma HLS ARRAY_PARTITION variable=layer13_out complete dim=0
    nnet::relu<layer11_t, layer13_t, relu_config13>(layer11_out, layer13_out); // qActivation_rho_1

    layer14_t layer14_out[N_LAYER_14];
    #pragma HLS ARRAY_PARTITION variable=layer14_out complete dim=0
    nnet::dense<layer13_t, layer14_t, config14>(layer13_out, layer14_out, w14, b14); // qDense_2_class

    layer16_t layer16_out[N_LAYER_16];
    #pragma HLS ARRAY_PARTITION variable=layer16_out complete dim=0
    nnet::dense<layer13_t, layer16_t, config16>(layer13_out, layer16_out, w16, b16); // qDense_2_reg

    layer18_t layer18_out[N_LAYER_14];
    #pragma HLS ARRAY_PARTITION variable=layer18_out complete dim=0
    nnet::relu<layer14_t, layer18_t, relu_config18>(layer14_out, layer18_out); // qActivation_2_class

    layer19_t layer19_out[N_LAYER_16];
    #pragma HLS ARRAY_PARTITION variable=layer19_out complete dim=0
    nnet::relu<layer16_t, layer19_t, relu_config19>(layer16_out, layer19_out); // qActivation_2_reg

    layer20_t layer20_out[N_LAYER_20];
    #pragma HLS ARRAY_PARTITION variable=layer20_out complete dim=0
    nnet::dense<layer18_t, layer20_t, config20>(layer18_out, layer20_out, w20, b20); // qDense_out_class

    nnet::dense<layer19_t, layer22_t, config22>(layer19_out, layer22_out, w22, b22); // qDense_out_reg

    nnet::softmax<layer20_t, layer24_t, softmax_config24>(layer20_out, layer24_out); // output_class

}
