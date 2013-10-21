# This file contains functions to parse neuroML and sbml.

import debug.debug as debug 
import logging 
import os
import importer 

logger = importer.logger

def parseModels(models) :

    """
    Parses given xml models. We can pass either one or two models; one described in
    neuroML and the other in sbml. 
    
    Notes: Document is properly. See 

      http://www.biomedcentral.com/1752-0509/7/88/abstract

    sent by Aditya Girla. It a online composition tool for SBML. In its
    references, some other tools are mentioned.

    Args :

    Raises :

    Return 
    return a list of elementTree of given models.

    """
    elemDict = dict()
    if models.nml :
        # Get the schema 
        with open(models.nml, "r") as nmlFile :
            elemDict['nml'] = models.nml
        
    if models.sbml :
        elemDict['sbml'] = models.sbml
    return elemDict

