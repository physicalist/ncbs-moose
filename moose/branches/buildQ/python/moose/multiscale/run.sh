#!/bin/bash
topFile=main.nw
runDoc() {
  ./pynoweb.py --top $topFile --weave "-delay > docs/$topFile.tex"
  #pdflatex -shell-escape -output-dir docs "./docs/$topFile.tex"
  # Run latexmk enough time to generate required document.
  cd docs && latexmk -ps main.nw
}

runCode() {
  ./pynoweb.py --top $topFile --tangle 
  python src/main.py --nml ./models/neuroML2/NML2_FullCell.nml \
    --adaptor ./moose_xml/adaptor.xml --mumbl ./moose_xml/mumbl.xml
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
