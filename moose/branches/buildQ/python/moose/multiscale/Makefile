###############################################################################
# Makefile for literate Python programes. It creates pdf and dvi documents as
# well as generate codes for execution,
#
# USAGE : make <target> file=<filename>
# <fileaname> must be without any extension. For example, if one wants to make
# example.snw for generating a dvi file then one should use following command.
# $ make dvi file=example
#
# (c) Dilawar, 2011
# Email : dilawar@ee.iitb.ac.in
# 
# June 30, 2011
###############################################################################
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
	python src/main.py

%.py : %.nw
	$(NOTANGLE) $< -R$@ > ./src/$@
	$(NOWEAVE)  $< > ./docs/$(<:.nw=.tex)

pdf : $(module_pdf)

%.pdf : %.nw
	$(NOWEAVE)  $< > ./docs/$($,.pdf=.tex)
	pdflatex -output-directory=docs ./docs/$@

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
