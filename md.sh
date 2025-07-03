#!/bin/bash

clear

cd src/dsp
make -B clean
make debug

cd ../adsr
make -B debug

cd ../lfo
make -B debug

cd ../jpvoice
make -B debug
