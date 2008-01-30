import squid
import threading
import sys
import numpy
from PyQt4 import QtGui, QtCore

import Queue

DT_MINIMUM = 1e-6
class SquidThread(QtCore.QThread):
    updateEvent = threading.Event()
    DO_RUN = 0
    DO_RESET = 1
    DO_QUIT = 2
    TOGGLE_NA = 3
    TOGGLE_K = 4
    def __init__(self, path, parent = None):
        self.squid = squid.Squid(path)
        self.exiting = False
        QtCore.QThread.__init__(self, parent)

    def doRun( self, totTime, preTime, updateTime, pulseWidth, injectBase, injectHigh ):
        # Here think of some cheating! Should we just doRun on squid,
        # or should we run till updateInterval and yield, and wait for
        # main thread to signal continuation?
        print "SquidThread - doRun(",  totTime, preTime, updateTime, pulseWidth, injectBase, injectHigh ,")"
        curTime = 0
        nextTime = updateTime
        while nextTime -preTime < DT_MINIMUM:
            # instead of using queue, try notifying the gui and wait for it to notify successful reading of data
            # But does that help in memory? Anyways, the queue entry will exist only till the gui has copied data
            print 'A',nextTime, preTime
            self.queue.put(self.squid.run(updateTime, injectBase))
            self.emit(QtCore.SIGNAL("output()"),())
            nextTime = nextTime + updateTime
            
        if ( nextTime - updateTime ) < preTime :
            print 'B', nextTime - updateTime, preTime
            vm = numpy.array(self.squid.run(preTime - nextTime + updateTime , injectBase))
            print "B) Done running"
            self.queue.put(vm)
            self.emit(QtCore.SIGNAL("output()"),())
            nextTime = preTime
            
        while nextTime < preTime + pulseWidth:
            print 'C', nextTime, preTime + pulseWidth
            self.queue.put(self.squid.run(updateTime, injectHigh))    
            self.emit(QtCore.SIGNAL("output()"),())
            nextTime = nextTime + updateTime
            
        if ( nextTime - updateTime ) < ( preTime + pulseWidth ):            
            print 'D', nextTime - updateTime, preTime + pulseWidth
            self.queue.put(self.squid.run(preTime +pulseWidth - nextTime + updateTime , injectHigh))
            self.emit(QtCore.SIGNAL("output()"),())
            nextTime = preTime + pulseWidth

        while nextTime < totTime:             
            print 'E', nextTime, totTime
            self.queue.put(self.squid.run(updateTime, injectHigh))    
            self.emit(QtCore.SIGNAL("output()"),())
            nextTime = nextTime + updateTime
        
        if nextTime - updateTime < totTime:
            print 'F', nextTime - updateTime, totTime
            self.queue.put(self.squid.run(totTime - nextTime + updateTime , injectBase))
            self.emit(QtCore.SIGNAL("output()"),())

    def doReset(self):
        self.squid.reset()

    
    def run(self):        
        while not self.exiting:
            print "SquidThread - trying to get an entry from queue"

    def __del__(self):
        self.exiting = True
        self.wait()

def main(args):
    thread = SquidThread('/test',None)
    thread.start()

if __name__ == "__main__":
    demo = SquidDemo()
    thread = SquidThread('/test',demo)
    thread.start()
    
