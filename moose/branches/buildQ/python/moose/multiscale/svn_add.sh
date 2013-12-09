#!/bin/bash
find . -type f -regex ".*\.\(sh\|py\|Make*\|xml\|xsd\|txt\|log\)" -print0 |\
     xargs -0 -I file svn -q add --parents file
