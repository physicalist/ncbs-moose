#!/bin/bash
topFile=main.nw
./pynoweb.py --top $topFile --tangle 
./pynoweb.py --top $topFile --weave "-delay > docs/$topFile.tex"
if [ "$1" == "--doc" ]; then 
  pdflatex -output-directory=docs "./docs/$topFile.tex"
else 
  python src/main.py --nml ./models/neuroML2/NML2_FullCell.nml \
    --adaptor ./moose_xml/adaptor.xml --mumbl ./moose_xml/mumbl.xml
fi
