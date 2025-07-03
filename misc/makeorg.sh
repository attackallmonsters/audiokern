#!/bin/bash

# creates the Purte Data extension based on the main branch on github
# script has to be executable

set -e

# remounting root filesystem read/write...
sudo mount -o remount,rw /

# removing old jpvoice directory
rm -r -f jpvoice

# cloning latest jpvoice from GitHub...
GIT_SSL_NO_VERIFY=true git clone https://github.com/attackallmonsters/jpvoice.git

# building release version
cd jpvoice || { echo "directory jpvoice not found"; exit 1; }
make release

echo "build complete"
