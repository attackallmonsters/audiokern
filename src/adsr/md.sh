#!/bin/bash

RUN_DEPS=false

# Parse -d Option
while getopts ":d" opt; do
  case ${opt} in
    d )
      RUN_DEPS=true
      ;;
    \? )
      echo "Unknown option: -$OPTARG" 1>&2
      exit 1
      ;;
  esac
done

echo "RUN_DEPS = $RUN_DEPS"

make clean

if [ "$RUN_DEPS" = "true" ]; then
  echo ">>> Building DSP module in ../dsp (debug)"
  make -C ../dsp clear
  make -C ../dsp debug || { echo "Error in ../dsp"; exit 1; }
fi

make clean
clear
make debug
