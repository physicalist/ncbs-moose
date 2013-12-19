import moose
import logging
import datetime
import time
import os
st = time.time()
st = datetime.datetime.fromtimestamp(st).strftime('%Y-%m-%d-%H%M')

logFile = 'logs/moose.log'
if os.path.exists(logFile):
    os.rename(logFile, 'logs/{0}'.format(st))

logging.basicConfig(filename=logFile, level=logging.DEBUG)
mooseLogger = logging.getLogger()

def logPathsToFille(pat):
   moose_paths = [x.getPath() for x in moose.wildcardFind(pat)]
   moose_paths = "\n".join(moose_paths)
   mooseLogger.debug(moose_paths)
