# NOTE

This functionality was migrated to [nifty](https://github.com/DerThorsten/nifty)

# NeuroMetrics

This repo bundles different metrics for evaluating segmentations (especially for neuron segmentation).
Implementation in C++ with python bindings.

Implemented:

* randIndex (implementation taken from Bjoern Andres)
* variationOfInformation (implementation taken from Bjoern Andres)
* randFScore (implementation adapted from https://github.com/fiji/Trainable_Segmentation/blob/master/src/main/java/trainableSegmentation/metrics/RandError.java)

## Installation

* run cmake in src
* run make in bld
* set pythonpath to bld/python

## TODO's

* proper installers (make install, conda)
* Tests
* Documentation
* more efficient: use same contingency matrix for calculating all measures
* Add randPrecision, randRecall and measures derived from VI
* change pybindings to pybind11
