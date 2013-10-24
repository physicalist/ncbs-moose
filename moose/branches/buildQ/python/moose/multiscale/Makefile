SHELL = /bin/sh
NOTANGLE = notangle
NOWEAVE = noweave # it implies -n option. Automatic latex wrapper will not be includeded in out tex file.
COMPILER = python
FLAGS = 
file = main
modules = parser.nw debug.nw main.nw
modules_py = $(modules:.nw=.py)
modules_pdf = $(modules:.nw=.pdf)

all : run

run : $(modules_py)
	echo "Build : $?"
	python src/main.py --nml=./models/neuroML2/Purk2M9s.nml

%.py : %.nw
	target=$<
	$(NOTANGLE) $< -R$@ > ./src/$@

pdf : $(modules_pdf) $(modules)

%.pdf : %.tex
	pdflatex -output-directory=docs ./docs/$<

%.tex : %.nw
	$(NOWEAVE)  $< > ./docs/$@

showdvi : 
	if $(shell test -e ./docs/$(file).dvi) \
	then $(shell xdvi ./docs/$(file).dvi) \
	else echo 'Run make dvi file=filename.' ;\
	fi

showpdf : 
	if $(shell test -e ./docs/$(file).pdf)\
	then $(shell xpdf ./docs/$(file).pdf) \
	else echo 'Run make pdf file=filename.' ;\
	fi

purge :
	rm -rf ./docs/* ./src/* 
