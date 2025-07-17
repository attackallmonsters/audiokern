#!/bin/bash

clear

cd src/audiokern
make -B clean
make debug

cd ../adsr
make -B debug

cd ../lfo
make -B debug

cd ../jpvoice
make -B debug
