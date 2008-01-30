from PyQt4 import QtGui, QtCore
from PyQt4 import Qwt5 as qwt
import PyQt4.Qwt5.qplt as qplt
import numpy
import squid
import sys

class SquidThread(QtCore.QThread):
    def __init__(self, sender, parent=None):
        QtCore.QThread.__init__(self, parent)
        self.sender = sender
        self.connect(self.sender, QtCore.SIGNAL("doRun"),self.doRun)
        self.connect(self, QtCore.SIGNAL("doUpdate"), self.sender.doUpdate)
        self.squid = squid.Squid("/squid")
        self.exiting = False

    def doRun(self, totalTime, prepulseTime, pulseWidth, inject, updateInterval):
        print "SquidThread.doRun(",totalTime, prepulseTime, pulseWidth, inject, updateInterval, ")"
        self.squid.doRun(totalTime, prepulseTime, pulseWidth, 0, inject)
        return
        updateTime = updateInterval
        currentTime = 0.0
        while updateTime < prepulseTime:
            vmTable = self.squid.run(updateInterval, 0)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime += updateInterval

        if updateTime - updateInterval < prepulseTime:
            vmTable = self.squid.run(prepulseTime - updateTime + updateInterval, 0)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime = prepulseTime + updateInterval

        pulseEnd = prepulseTime + pulseWidth
        while updateTime < pulseEnd:            
            vmTable = self.squid.run(updateInterval, inject)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime += updateInterval

        if updateTime - updateInterval < pulseEnd:
            vmTable = self.squid.run(pulseEnd - updateTime + updateInterval, inject)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime = pulseEnd + updateInterval

        while updateTime < totalTime:
            vmTable = self.squid.run(updateInterval, 0)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime += updateInterval

        if updateTime - updateInterval < totalTime:
            vmTable = self.squid.run(totalTime - updateTime + updateInterval, 0)
            self.emit(QtCore.SIGNAL("doUpdate"),(vmTable))
            updateTime = pulseEnd + updateInterval

    def doReset(self):
        print "SquidThread.doReset()"
        self.squid.reset()

    def doExit(self):
        self.exiting = True

    def run(self):
        while not self.exiting:
            print "SquidThread.run()"
            self.usleep(1000)

    def __del__(self):
        self.exiting = True
        self.wait()

class SquidWidget(QtGui.QMainWindow):
    def __init__(self, *args):
        apply(QtGui.QMainWindow.__init__, (self,)+args)
        self.setWindowTitle('MOOSE Squid Demo')
        self.mainWidget = QtGui.QWidget()
        self.setCentralWidget(self.mainWidget)
        self.resize(600,400)
        
        self.initActions()
        self.initMenuBar()
        self.initToolBar()
        self.statusBar()
        
        self.squidThread = SquidThread(self)
        self.connect(self, QtCore.SIGNAL("doExit"), self.squidThread.doExit)
        self.squidThread.start()

    def initActions(self):
        self.actions = {}
        # Create the actions and insert into dictionary
        self.actions["exit"] =  QtGui.QAction("Exit", self)
        self.actions["run"] = QtGui.QAction("Run", self)
        self.actions["reset"] = QtGui.QAction("Reset", self)
        # Set key-board shortcuts
        self.actions["exit"].setShortcut("Ctrl+Q")
        self.actions["run"].setShortcut("Ctrl+X")
        self.actions["reset"].setShortcut("Ctrl+R")
        # Set tool-tip text
        self.actions["exit"].setStatusTip("Exit application")   
        self.actions["run"].setStatusTip("Run the squid demo")     
        self.actions["reset"].setStatusTip("Reset the simulation")
        # Connect the actions to correct slots
        self.connect(self.actions["exit"], QtCore.SIGNAL("triggered()"), self.doExit)
        self.connect(self.actions["run"], QtCore.SIGNAL("triggered()"), self.slotRun)
        self.connect(self.actions["reset"], QtCore.SIGNAL("triggered()"), self.slotReset)

    def initMenuBar(self):
        self.menu = QtGui.QMenuBar()
        file = self.menu.addMenu("&File")
        file.addAction(self.actions["exit"])
        file.addAction(self.actions["run"])
        file.addAction(self.actions["reset"])

    def initToolBar(self):        
        self.toolbar = self.addToolBar("Exit")
        self.toolbar.addAction(self.actions["exit"])
        self.toolbar = self.addToolBar("Run")
        self.toolbar.addAction(self.actions["run"])
        self.toolbar = self.addToolBar("Reset")
        self.toolbar.addAction(self.actions["reset"])

    def doExit(self):
        self.emit(QtCore.SIGNAL("doExit"), ())
    
    def slotRun(self):
        totalTime = 0.05
        prepulseTime = 0.02
        pulseWidth = 0.03
        inject = 1.0e-6
        updateInterval  = 0.01        
        self.emit(QtCore.SIGNAL("doRun"), totalTime, prepulseTime, pulseWidth, inject, updateInterval)

    def slotReset(self):
        self.emit(QtCore.SIGNAL("doReset"),())

    def doUpdate(self, vm):
        print "SquidWidget.doUpdate() - Received new data of length ", len(vm)

    

def main(*args):   
    app = QtGui.QApplication(sys.argv)
    mainWindow = SquidWidget()
    mainWindow.show()
    app.connect(app, QtCore.SIGNAL("lastWindowClosed()"), app, QtCore.SLOT("quit()"))
    app.exec_()


if __name__ == "__main__":
    main(sys.argv)
