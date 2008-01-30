import squid
import threading
import sys
import numpy
from PyQt4 import QtGui, QtCore

import Queue

DT_MINIMUM = 1e-6
class SquidThread(threading.Thread):
    updateEvent = threading.Event()
    DO_RUN = 0
    DO_RESET = 1
    DO_QUIT = 2
    TOGGLE_NA = 3
    TOGGLE_K = 4
    def __init__(self, path, consumer):
        self.queue = Queue.Queue()
        self.squid = squid.Squid(path)
        self.consumer = consumer
        threading.Thread.__init__(self)
        
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
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()
            nextTime = nextTime + updateTime
            
        if ( nextTime - updateTime ) < preTime :
            print 'B', nextTime - updateTime, preTime
            vm = numpy.array(self.squid.run(preTime - nextTime + updateTime , injectBase))
            print "B) Done running"
            self.queue.put(vm)
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()
            nextTime = preTime
            
        while nextTime < preTime + pulseWidth:
            print 'C', nextTime, preTime + pulseWidth
            self.queue.put(self.squid.run(updateTime, injectHigh))    
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()
            nextTime = nextTime + updateTime
            
        if ( nextTime - updateTime ) < ( preTime + pulseWidth ):            
            print 'D', nextTime - updateTime, preTime + pulseWidth
            self.queue.put(self.squid.run(preTime +pulseWidth - nextTime + updateTime , injectHigh))
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()
            nextTime = preTime + pulseWidth

        while nextTime < totTime:             
            print 'E', nextTime, totTime
            self.queue.put(self.squid.run(updateTime, injectHigh))    
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()
            nextTime = nextTime + updateTime
        
        if nextTime - updateTime < totTime:
            print 'F', nextTime - updateTime, totTime
            self.queue.put(self.squid.run(totTime - nextTime + updateTime , injectBase))
#             SquidThread.updateEvent.set()
#             SquidThread.updateEvent.clear()

    def doReset(self):
        self.squid.reset()

    
    def run(self):
        keepRunning = True
        while keepRunning:
            try:
                print "SquidThread - trying to get an entry from queue"
                inputs = self.consumer.requestQueue.get(True, 0.1)
                if len(inputs) > 0:
                    if inputs[0] == SquidThread.DO_RESET:
                        print "SquidThread - Resetting"
                        self.doReset()
                    elif inputs[0] == SquidThread.DO_RUN:
                        print "SquidThread - Doing simulation"
                        self.doRun(inputs[1], inputs[2], inputs[3], inputs[4], inputs[5], inputs[6])
                        print "SquidThread - Done simulation"
                    elif inputs[0] == SquidThread.DO_QUIT:
                        keepRunning = False
                        print "SquidThread - Exiting"
                    elif inputs[0] == SquidThread.TOGGLE_NA:
                        self.squid.toggelNaChannels()
                        print "SquidThread - Toggle Na channels"
                    elif inputs[0] == SquidThread.TOGGLE_K:
                        self.squid.toggelKChannels()
                        print "SquidThread - Toggle K Channels"
                    else:
                        print "SquidThread - Unknown option"
            except Queue.Empty, e:
                print "Empty queue", e

def main(args):
    thread = SquidThread('/test',None)
    thread.start()

if __name__ == "__main__":
    demo = SquidDemo()
    thread = SquidThread('/test',demo)
    thread.start()
    
