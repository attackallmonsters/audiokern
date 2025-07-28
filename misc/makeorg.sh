#!/bin/bash

# creates the Pure Data extension based on the given project name from GitHub
# usage: ./build.sh ladder

set -e

# check if a parameter was given
if [[ -z "$1" ]]; then
    echo "Usage: $0 <project-name>"
    exit 1
fi

PROJECT="$1"

# remounting root filesystem read/write...
sudo mount -o remount,rw /

# removing old directory
rm -r -f "audiokern"

# cloning latest from GitHub...
GIT_SSL_NO_VERIFY=true git clone "https://github.com/attackallmonsters/audiokern.git"

# building audiokern release version
cd "audiokern/src/audiokern" || { echo "directory audiokern not found"; exit 1; }
make release

# building release version
cd "../$PROJECT" || { echo "directory $PROJECT not found"; exit 1; }
make release

echo "build complete"
