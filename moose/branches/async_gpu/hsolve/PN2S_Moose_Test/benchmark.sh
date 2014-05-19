#!/bin/sh
python -m yep testHsolve.py
pprof --gv ../async_gpu/python/moose/_moose.so testHsolve.py.prof 
