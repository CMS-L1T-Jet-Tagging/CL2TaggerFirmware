# CL2TaggerFirmware

## Conda environment

```
conda-env create -f environment.yml
```

Activate the environment:

```
conda activate L1_Tagger
```

And then do whatever you want in this environment (edit files, open notebooks, etc.). To deactivate the environment:

```
conda deactivate
```

If you make any update for the environment, please edit the `environment.yml` file and run:

```
conda env update --file environment.yml  --prune
```