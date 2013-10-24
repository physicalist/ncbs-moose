#!/bin/bash
topFile=main.nw
./pynoweb.py --top $topFile --tangle 
./pynoweb.py --top $topFile --weave "-delay > docs/$topFile.tex"
if [ "$1" == "--doc" ]; then 
  pdflatex -output-directory=docs "./docs/$topFile.tex"
else 
  python src/main.py --xml ./moose_xml/test_xml/multiscale1.xml
fi
