#!/bin/bash

RUN_DEPS=false

clear;

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
  echo ">>> Building audiokern module in ../audiokern (release)"
  make -C ../audiokern clear
  make -C ../audiokern release || { echo "Error in ../audiokern"; exit 1; }
fi

make clean
make release
