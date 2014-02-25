#!/bin/bash
echo "Updaing async13 branch"
( cd ../async13/ && svn up )
svn merge ../async13/
