# CL2TaggerFirmware

## Set up CMSSW

CMSSW is needed to pull the data types for building the hls project

```
cmsrel CMSSW_14_0_0_pre3
cd CMSSW_14_0_0_pre3/src
cmsenv
git cms-init

# Get relevant packages
git cms-addpkg DataFormats/L1TParticleFlow
git cms-addpkg DataFormats/L1TCorrelator
git cms-addpkg L1Trigger/Phase2L1ParticleFlow
git cms-addpkg L1Trigger/TrackTrigger
git cms-addpkg SimTracker/TrackTriggerAssociation

# Latest integration branch
git cms-checkout-topic -u cms-l1t-offline:phase2-l1t-1400pre3_v9
```

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