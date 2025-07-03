#!/bin/bash

clear

cd src/dsp
make -B release

cd ../adsr
make -B release

cd ../lfo
make -B release

cd ../jpvoice
make -B release
