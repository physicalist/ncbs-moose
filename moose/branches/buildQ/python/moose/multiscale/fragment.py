#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"

__log__     = """


"""

import re 
import sys 
filename = "./moose_xml/NeuroML_v2beta1.xsd"
tag = sys.argv[1]
totalLine = int(sys.argv[2])

startCollection = False
outtext = []
with open(filename, "r") as nml :
  for line in nml :
    regex = re.compile(r"\<xs:(?P<tag>\w+)\s+name=\"{0}\"\>".format(tag) 
         , re.I | re.S)
    m = regex.search(line)
    if m :
      startCollection = True 
    if startCollection and totalLine >= 0 :
      totalLine -= 1
      outtext.append(line)


fragment = ["\\begin{lstlisting}[language=XML]\n"] + outtext \
    + ["\\end{lstlisting}\n"] 

print "".join(fragment)
