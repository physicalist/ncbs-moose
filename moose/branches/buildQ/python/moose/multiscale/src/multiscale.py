# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree

import sqlite3 as sql 

class Multiscale :
  
  def __init__(self, xmlDict) :
    self.xmlDict = xmlDict 
    self.dbdir = 'db'
    self.dbname = 'models.db'
    self.dbpath = os.path.join(self.dbdir, self.dbname)
    if not os.path.exists(self.dbpath) :
      try :
        os.makedirs(self.dbdir)
      except Exception as e :
        debug.printDebug("ERROR"
            , "Faild to create directory {0} with error {1}".format(self.dbdir, e))
        sys.exit(0)
    self.conn = sql.connect(self.dbpath)
    self.cursor = self.conn.cursor()
    debug.printDebug("INFO", "Object of class Multiscale intialized ...")

  

  # This is the entry point of this class.
  def buildMultiscaleModel(self) :
      debug.printDebug("INFO", "Starting to build multiscale model")   
      


  
  def exit(self) :
    # Clean up before you leave
    self.cursor.commit()
    self.conn.close()
    sys.exit()

  # Write down the tests, whenever needed.
  




