#!/bin/bash
set -x
set -e

if [ ! -f ./_service:recompress:tar_scm:moose-3.0.1.tar.bz2 ]; then
    osc service run
else
    osc build --noservice
fi
