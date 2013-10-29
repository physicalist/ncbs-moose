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

  def extractTransformLoad(self, modelType, xmlRootNode) :
    if modelType == 'nml' :
      self.etlNMLModel(xmlRootNode)
    else :
        pass 

  def etlNMLModel(self, nmlRootNode) :
    debug.printDebug("STEP", "ETLing a nml model")
    for child in nmlRootNode.iter() :
      print child
   
  def initDB(self) :
    query = '''DROP TABLE IF EXISTS segment'''
    self.cursor.execute(query)
    query = '''CREATE TABLE IF NOT EXISTS segment 
      (id INTEGER PRIMARY KEY ASC
      , name VARCHAR
      , parent INTEGER 
      , proximal REAL
      , distal REAL
      , x REAL 
      , y REAL  
      , z REAL 
      , remark TEXT)'''
    self.cursor.execute(query)
    query = 'DROP TABLE IF EXISTS cells'
    self.cursor.execute(query)
    query = '''CREATE TABLE IF NOT EXISTS cells
      (type VARCHAR PRIMARY KEY -- Type of cell
      , leakReversal REAL       -- leakReversal potential
      , threshold REAL          -- Threshold voltage
      , reset REAL
      , tau REAL
      , refract REAL
      , capacitance REAL
      , leakConductance REAL
      , a REAL                  -- Izhikenvich Cell model
      , b REAL                  -- Izhikenvich cell model
      , c REAL                  -- Izhikenvich cell model 
      , d REAL                  -- Izhikenvich cell model
      , gL REAL
      , EL REAL
      , reset REAL
      , VT REAL
      , delT REAL
      , tauw REAL
      , Idel REAL
      , Idur REAL
      )'''


  # This is the entry point of this class.
  def buildMultiscaleModel(self) :
      debug.printDebug("INFO", "Starting to build multiscale model")   
      self.initDB()
      for xml in self.xmlDict :
        xmlRootNodeList = self.xmlDict[xml]
        for xmlRootNode in xmlRootNodeList :
          self.extractTransformLoad(xml, xmlRootNode)


  
  def exit(self) :
    # Clean up before you leave
    self.cursor.commit()
    self.conn.close()
    sys.exit()

  # Write down the tests, whenever needed.
  




