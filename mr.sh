#!/bin/bash

clear

cd src/audiokern
make -B release

###### TODO, does not build currently
# cd ../adsr
# make -B release

# cd ../lfo
# make -B release

cd ../jpsynth
make -B release
