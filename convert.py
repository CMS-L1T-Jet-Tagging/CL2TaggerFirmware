import hls4ml
import os, sys
import json
from argparse import ArgumentParser
from qkeras.utils import load_qmodel

hls4ml_abspath = os.path.abspath('hls4ml')

#Add editable hls4ml installation
os.system(f'export PYTHONPATH=$PYTHONPATH:{hls4ml_abspath}')

print(f'export PYTHONPATH=$PYTHONPATH:{hls4ml_abspath}')

#----------------------------------------------

def convert(model, outname):

    #Remove the old directory if they exist
    os.system(f'rm -rf {outname}')

    #Auxilary variables
    input_precision = 'ap_fixed<15,12,AP_RND,AP_SAT>'
    class_precision = 'ap_ufixed<8,0,AP_RND,AP_SAT>'
    reg_precision = 'ap_fixed<10,6,AP_RND,AP_SAT>' 
    trace=True

    #Create default config
    config = hls4ml.utils.config_from_keras_model(model, granularity='name')
    config['IOType'] = 'io_parallel'
    config['LayerName']['inputs']['Precision']['result'] = input_precision

    #Additional config
    for layer in model.layers:
        layer_name = layer.__class__.__name__

        if layer_name in ["BatchNormalization", "InputLayer"]:
            config["LayerName"][layer.name]["Precision"] = input_precision
            config["LayerName"][layer.name]["result"] = input_precision
            config["LayerName"][layer.name]["Trace"] = trace

        elif layer_name in ["Permute","Concatenate","Flatten","Reshape","UpSampling1D","Add"]:
            print("Skipping trace for:", layer.name)
        else:
            config["LayerName"][layer.name]["Trace"] = trace

    
    config["LayerName"]["output_class"]["Precision"]["result"] = class_precision
    config["LayerName"]["output_reg"]["Precision"]["result"] = reg_precision
    config["LayerName"]["output_class"]["Implementation"] = "latency"
    config["LayerName"]["output_reg"]["Implementation"] = "latency"

    #Save config  as json file
    print("Saving default config as config.json ...")
    with open('config.json', 'w') as fp: json.dump(config, fp)

    #Write HLS
    hls_model = hls4ml.converters.convert_from_keras_model(model,
                                                       backend='Vitis',
                                                       project_name='JetTaggerNN',
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
    parser.add_argument('-o','--outname', default='JetTaggerNN' , help = 'Jet tagger synthesized output directory')    

    args = parser.parse_args()

    model_path = f'models/{args.model}'

    model=load_qmodel(model_path)
    print(model.summary())

    convert(model, args.outname)