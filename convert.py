import hls4ml
import os, sys
from argparse import ArgumentParser
from qkeras.utils import load_qmodel

#Vivado path
os.environ['PATH'] += os.pathsep + '/data/Xilinx/Vivado/2023.2/bin'
#----------------------------------------------

def convert(model, outname):

    #Remove the old directory if they exist
    os.system(f'rm -rf {outname}')

    #Create default config
    config = hls4ml.utils.config_from_keras_model(model, granularity='name')
    config['IOType'] = 'io_parallel'

    inputPrecision = "ap_ufixed<14,12,AP_RND,AP_SAT>"

    hls_model = hls4ml.converters.convert_from_keras_model(model,
                                                       backend='Vitis',
                                                       project_name='JetTagger',
                                                       clock_period=2.8, #1/360MHz = 2.8ns
                                                       hls_config=config,
                                                       output_dir=f'{outname}',
                                                       part='xcvu9p-flga2104-2L-e')

    #Compile and build the project
    hls_model.compile()
    hls_model.build(csim=False, reset = True)

if __name__ == "__main__":

    parser = ArgumentParser()
    parser.add_argument('-m','--model', default='model_QDeepSets_PermutationInv_nconst_16_nfeatures_21_nbits_8_pruned.h5' , help = 'Input model for conversion')    
    parser.add_argument('-o','--outname', default='JetTagger' , help = 'Jet tagger synthesized output directory')    

    args = parser.parse_args()

    model_path = f'models/{args.model}'

    model=load_qmodel(model_path)
    print(model.summary())

    convert(model, args.outname)