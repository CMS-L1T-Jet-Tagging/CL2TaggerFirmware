#ifndef JET_TAGGER_H
#define JET_TAGGER_H

#include <ap_int.h>
#include <hls_stream.h>
#include "data.h"

void JetTagger(const Particle input[N_JET_PARTICLES], Jet output[NOUTPUT], JetCtrlToken& token_d, JetCtrlToken& token_q);

#endif