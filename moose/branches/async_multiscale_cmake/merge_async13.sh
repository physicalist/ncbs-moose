#!/bin/bash
echo "Updaing async13 branch"
( cd ../async13/ && svn up )
echo "Create patch"
svn diff ../async13/ . > patch_asyc13_this.patch
echo "Applying patch"
svn patch patch_asyc13_this.patch 
