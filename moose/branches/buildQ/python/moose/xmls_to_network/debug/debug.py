from __future__ import print_function
import inspect 
import sys

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKGREEN = '\033[92m'
WARNING = '\033[93m'
ERR = '\033[31m'
ENDC = '\033[0m'
RED = ERR
WARN = WARNING
INFO = OKBLUE
TODO = OKGREEN
DEBUG = HEADER
ERROR = ERR

prefix = dict(
    ERR = ERR
    , ERROR = ERR
    , WARN = WARN
    , FATAL = ERR
    , INFO = INFO
    , TODO = TODO 
    , NOTE = HEADER 
    , DEBUG = DEBUG
    )

def colored(msg, label="INFO") :
    """
    Return a colored string. Formatting is optional.
    """
    global prefix
    if label in prefix :
        color = prefix[label]
    else :
        color = ""
    return "{0} {1}".format(color+msg, ENDC)

def printDebug(label, msg, frame=None):
    if not frame :
      print("[{0}] {1}".format(label, colored(msg,label)), file=sys.stderr)
    else :
      filename = frame.f_code.co_filename 
      filename = "/".join(filename.split("/")[-2:])
      print("[{3}] @...{0}:{1} {2}".format(filename
        , frame.f_lineno
        , colored(msg, label)
        , label)
        , file=sys.stderr
        )

