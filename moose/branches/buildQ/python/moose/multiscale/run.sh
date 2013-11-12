#!/bin/bash
topFile=main.nw
runDoc() {
  ./pynoweb.py --top $topFile --weave "-delay > docs/$topFile.tex"
  #pdflatex -shell-escape -output-dir docs "./docs/$topFile.tex"
  # Run latexmk enough time to generate required document.
  cd docs && latexmk -ps --shell-escape main.nw
}

runCode() {
  ./pynoweb.py --top $topFile --tangle 
  python src/main.py --nml ./models/neuroml/v2/NML2_FullCell.nml \
    --adaptor ./schema/adaptor/adaptor.xml \
    --mumbl ./schema/mumbl/mumbl.xml
  neato -Teps graphs/moose.dot > graphs/moose.eps
  dot -Teps graphs/moose.dot > graphs/topology.eps
}

if [ "$1" == "--doc" ]; then 
  runDoc
elif [ "$1" == "--all" ]; then 
  runCode
  runDoc
else 
  runCode
fi
