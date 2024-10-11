#include "algo_b.h"
#include "defines.h"
#include "btag_nn.h"
#include "data.h"
#include "synchronizer.h"

template<unsigned int N>
void make_inputs(input_t (&btag_ingput)[N*15], const Particle puppi_candidate[N]) {
  //Based on latest PUPPI specifications
  #pragma HLS INLINE

  for (int i = 0; i < N; i++) {

    Particle tmpobj = puppi_candidate[i];

    // One-hot encode the particle id

    btag_ingput[i*13+0] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::ELEMINUS ? 1 : 0); //e-
    btag_ingput[i*13+1] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::ELEPLUS ? 1 : 0); //e+
    btag_ingput[i*13+2] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::MUMINUS ? 1 : 0); //u-
    btag_ingput[i*13+3] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::MUPLUS ? 1 : 0); //u+
    btag_ingput[i*13+4] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::PHOTON ? 1 : 0); //photon
    btag_ingput[i*13+5] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::HADZERO ? 1 : 0); //kL
    btag_ingput[i*13+6] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::HADPLUS ? 1 : 0); //pi+
    btag_ingput[i*13+7] = input_t(tmpobj.hwId.bits == l1ct::ParticleID::HADMINUS ? 1 : 0); //pi-

    if ((tmpobj.hwId.bits == l1ct::ParticleID::PHOTON) || (tmpobj.hwId.bits == l1ct::ParticleID::HADZERO)){
      //Neutral particles
      btag_ingput[i*13+8] = input_t(0);//z0
      btag_ingput[i*13+9] = input_t(0);//DXY
    }
    else
    {
      //Charged Particles
      btag_ingput[i*13+8] = input_t(tmpobj.hwZ0())*input_t(l1ct::Scales::Z0_LSB);//z0
      btag_ingput[i*13+9] = input_t(tmpobj.hwDxy())*input_t(l1ct::Scales::DXY_LSB);//DXY
    }

    //Common Particle Properties
    btag_ingput[i*13+10] = input_t(tmpobj.hwPt); //pT 
    btag_ingput[i*13+11] = input_t(tmpobj.hwEta)*input_t(l1ct::Scales::ETAPHI_LSB); //Eta
    btag_ingput[i*13+12] = input_t(tmpobj.hwPhi)*input_t(l1ct::Scales::ETAPHI_LSB); //Phi
    
  }

  //Padd the rest with zeros
  for (int j = 131; j<N*15; j++){
    btag_ingput[j] = 0;
  }
} 

void JetTagger(const Particle input[N_JET_PARTICLES], Jet output[NOUTPUT], const JetCtrlToken& token_d, JetCtrlToken& token_q) { 
  #pragma HLS ARRAY_PARTITION variable=input complete
  #pragma HLS ARRAY_PARTITION variable=output complete

  #pragma HLS INTERFACE mode=ap_none port=output,token_q

  #pragma HLS PIPELINE II=1

  // Pass on the token
  token_q = token_d;
  
  // Take the inputs and aggregate it for the model
  input_t  btag_input[N_JET_PARTICLES*15];
  result_t btag_output[NOUTPUT];
  #pragma HLS ARRAY_PARTITION variable=btag_input complete
  #pragma HLS ARRAY_PARTITION variable=btag_output complete

  make_inputs<N_JET_PARTICLES>(btag_input, input);
  btag_nn(btag_input, btag_output); // Run it through the network
  
  //clear the output and assign the score
  clear(output[0]);
  output[0].hwBtagScore = b_tag_score_t(btag_output[0]); // Might need to deal with this
}
