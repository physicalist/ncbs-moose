# used to parse files more easily
from __future__ import with_statement

#numerical computations
import numpy as np

# for command-line arguments
import sys
import os
import sys
import subprocess
from datetime import date

#moose imports
import moose
import moose.utils as mooseUtils
from collections import defaultdict
from objectedit import ObjectFieldsModel, ObjectEditView
from moosehandler import MooseHandler
from mooseplot import MoosePlot,MoosePlotWindow

import kineticlayout
from neuralLayout import *

from filepaths import *
from defaults import *

# Qt4 bindings for Qt
from PyQt4 import QtCore,QtGui
from PyQt4.QtCore import QEvent, Qt

# import the MainWindow widget from the converted .ui (pyuic4) files
from newgui import Ui_MainWindow

import config


class DesignerMainWindow(QtGui.QMainWindow, Ui_MainWindow):
    """Customization for Qt Designer created window"""
    def __init__(self, interpreter=None,parent = None):
        # initialization of the superclass
        super(DesignerMainWindow, self).__init__(parent)
        # setup the GUI --> function generated by pyuic4
        self.objFieldEditorMap = {}
        self.setupUi(self)
        self.setCorner(Qt.BottomRightCorner,Qt.RightDockWidgetArea)
        self.setCorner(Qt.BottomLeftCorner,Qt.LeftDockWidgetArea)
        self.mooseHandler = MooseHandler()

        #other variables
        self.currentTime = 0.0
        self.allCompartments = []
        self.allIntFires = []
        self.modelHasCompartments = False
        self.modelHasIntFires = False
        self.modelPathsModelTypeDict = {}
        #prop Editor variables
        self.propEditorCurrentSelection = None
        self.propEditorChildrenIdDict = {}

        #plot config variables
        self.plotConfigCurrentSelection = None
        self.plotConfigAcceptPushButton.setEnabled(False)
        self.plotWindowFieldTableDict = {} #guiPlotWinowName:[mooseTable]
        self.plotNameWinDict = {} #guiPlotWindowName:moosePlotWindow

        #do not show other docks
        self.defaultDockState()

        #connections
        self.connectElements()

    def defaultDockState(self):
        #this will eventually change corresponding to the "mode" of operation - Edit/Plot/Run
        self.moosePopulationEditDock.setVisible(False)
        self.mooseLibraryDock.setVisible(False)
        self.mooseConnectDock.setVisible(False)

        self.menuHelp.setEnabled(False)
        self.menuView.setEnabled(False)
        self.menuClasses.setEnabled(False)

    def connectElements(self):
        #gui connections
        self.connect(self.actionLoad_Model,QtCore.SIGNAL('triggered()'), self.popupLoadModelDialog)
        self.connect(self.actionQuit,QtCore.SIGNAL('triggered()'),self.doQuit)
        #plotdock connections
        self.connect(self.plotConfigAcceptPushButton,QtCore.SIGNAL('pressed()'),self.addFieldToPlot)
        self.connect(self.plotConfigNewWindowPushButton,QtCore.SIGNAL('pressed()'),self.plotConfigAddNewPlotWindow)
        #propEditor connections
        self.connect(self.propEditorSelParentPushButton,QtCore.SIGNAL('pressed()'),self.propEditorSelectParent)
        self.connect(self.propEditorChildListWidget,QtCore.SIGNAL('itemClicked(QListWidgetItem *)'),self.propEditorSelectChild)
        #internal connections
        self.connect(self.mooseHandler, QtCore.SIGNAL('updatePlots(float)'), self.updatePlots)
        #run
        self.connect(self.simControlRunPushButton, QtCore.SIGNAL('clicked()'), self._resetAndRunSlot)
        self.connect(self.actionRun,QtCore.SIGNAL('triggered()'),self._resetAndRunSlot)
        self.connect(self.simControlContinuePushButton, QtCore.SIGNAL('clicked()'), self._continueSlot)
        self.connect(self.actionContinue,QtCore.SIGNAL('triggered()'),self._continueSlot)
        self.connect(self.simControlResetPushButton, QtCore.SIGNAL('clicked()'), self._resetSlot)
        self.connect(self.actionReset,QtCore.SIGNAL('triggered()'),self._resetSlot)

    def popupLoadModelDialog(self):
        fileDialog = QtGui.QFileDialog(self)
        fileDialog.setFileMode(QtGui.QFileDialog.ExistingFile)
        ffilter =''
        for key in sorted(self.mooseHandler.fileExtensionMap.keys()):
            ffilter = ffilter + key + ';;'
        ffilter = ffilter[:-2]
        fileDialog.setFilter(self.tr(ffilter))
        fileDialog.setWindowTitle('Open File')

        targetPanel = QtGui.QFrame(fileDialog)
        targetPanel.setLayout(QtGui.QVBoxLayout())

        currentPath = self.mooseHandler._current_element.path
        
        targetLabel = QtGui.QLabel('Target Element')
        targetText = QtGui.QLineEdit(fileDialog)
        
        targetText.setText(currentPath)
        targetText.setText('/')
        targetText.setReadOnly(1)
        targetPanel.layout().addWidget(targetLabel)
        targetPanel.layout().addWidget(targetText)
        layout = fileDialog.layout()
        layout.addWidget(targetPanel)
        self.connect(fileDialog, QtCore.SIGNAL('currentChanged(const QString &)'), lambda path:(targetText.setText(os.path.basename(str(path)).rpartition('.')[0])) )
        #self.connect(targetText, QtCore.SIGNAL('textChanged(const QString &)'), self.getElementpath)

        if fileDialog.exec_():
            fileNames = fileDialog.selectedFiles()
            fileFilter = fileDialog.selectedFilter()
            fileType = self.mooseHandler.fileExtensionMap[str(fileFilter)]

            if fileType == self.mooseHandler.type_all:
                reMap = str(fileNames[0]).rsplit('.')[1]
                if reMap == 'g':
                    fileType = self.mooseHandler.type_genesis
                elif reMap == 'py':
                    fileType = self.mooseHandler.type_python
                else:
                    fileType = self.mooseHandler.type_xml

            directory = fileDialog.directory()
            #self.statusBar.showMessage('Loading model, please wait')
            app = QtGui.qApp
            app.setOverrideCursor(QtGui.QCursor(Qt.BusyCursor)) #shows a hourglass - or a busy/working arrow
            for fileName in fileNames:
                if(((str(targetText.text())) == '/') or ((str(targetText.text())) == ' ') ):
                    modelpath = os.path.basename(str(fileName)).rpartition('.')[0]
                else:
                    modelpath = str(targetText.text())
                modeltype  = self.mooseHandler.loadModel(str(fileName), str(fileType), modelpath)
                if modeltype == MooseHandler.type_kkit:
                    try:
                        self.addKKITLayoutWindow(modelpath)
                        self.actionLoad_Model.setEnabled(0) #to prevent multiple loads
                        
                    except kineticlayout.Widgetvisibility:
                        print 'No kkit layout for: %s' % (str(fileName))
                    self.populateKKitPlots(modelpath)
                self.modelPathsModelTypeDict[modelpath] = modeltype
                self.populateDataPlots(modelpath)
                self.updateDefaultTimes(modeltype)
            #self.enableControlButtons()
            self.checkModelForNeurons()
            if self.modelHasCompartments or self.modelHasIntFires:
                self.addGLWindow()
            self.assignClocks(modelpath,modeltype)
            print 'Loaded model',  fileName, 'of type', modeltype, 'under Element path ',modelpath
            app.restoreOverrideCursor()

    def assignClocks(self,modelpath,modeltype):
        if modeltype == MooseHandler.type_kkit:
            #self.mooseHandler.updateClocks(MooseHandler.DEFAULT_SIMDT_KKIT, MooseHandler.DEFAULT_PLOTDT_KKIT)
            #script auto asigns clocks!
            pass
        elif modeltype == MooseHandler.type_neuroml:
            #self.mooseHandler.updateClocks(MooseHandler.DEFAULT_SIMDT, MooseHandler.DEFAULT_PLOTDT)
            #use Aditya's method to assign clocks - also reinits!
            mooseUtils.resetSim(['/cells','/elec'], MooseHandler.DEFAULT_SIMDT, MooseHandler.DEFAULT_PLOTDT)
        elif modeltype == MooseHandler.type_python:
            #specific for the hopfield tutorial!
            #self.mooseHandler.updateClocks(MooseHandler.DEFAULT_SIMDT, MooseHandler.DEFAULT_PLOTDT)
            clock = 1e-4
            self.simControlSimdtLineEdit.setText(str(clock))
            self.simControlPlotdtLineEdit.setText(str(clock))
            self.mooseHandler.updateClocks(clock, clock) #simdt,plotdt
            moose.useClock(1, '/hopfield/##[TYPE=IntFire]', 'process')
            moose.useClock(2, '/hopfield/##[TYPE=PulseGen]', 'process')
            moose.useClock(2, '/hopfield/##[TYPE=SpikeGen]', 'process')
            moose.useClock(4, '/hopfield/##[TYPE=Table]', 'process') 
        else:
            print 'Clocks have not been assigned! - GUI does not supported this format yet'

    def checkModelForNeurons(self):
        self.allCompartments = mooseUtils.findAllBut('/##[TYPE=Compartment]','library')
        self.allIntFires = mooseUtils.findAllBut('/##[TYPE=IntFire]','library')
        if self.allCompartments:
            self.modelHasCompartments = True
        if self.allIntFires:
            self.modelHasIntFires = True

    def addGLWindow(self):
	self.layoutWidget = updatepaintGL(self.centralwidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.centralwidget.sizePolicy().hasHeightForWidth())
        self.layoutWidget.setSizePolicy(sizePolicy)
        self.layoutWidget.setObjectName("layoutWidget")
        self.horizontalLayout.addWidget(self.layoutWidget)
        self.centralwidget.setSizePolicy(sizePolicy)
        self.centralwidget.setObjectName("layoutWidget")
        self.centralwidget.show()

        QtCore.QObject.connect(self.layoutWidget,QtCore.SIGNAL('compartmentSelected(QString)'),self.pickCompartment)
        self.updateCanvas()

    def pickCompartment(self, name):
        self.makeObjectFieldEditor(moose.element(str(name)))

    def updateVisualization(self):
        self.layoutWidget.updateViz()

    def updateCanvas(self):
        cellNameComptDict = {}
        self.layoutWidget.viz = 1 #not the brightest way to go about
        if self.modelHasCompartments:
            for compartment in self.allCompartments: 
                cellName = compartment.getField('parent').getField('path') #enforcing a hierarchy /cell/compartment! - not a good idea 
                if cellName in cellNameComptDict:
                    cellNameComptDict[cellName].append(compartment)
                else:
                    cellNameComptDict[cellName] = [compartment]
            for cellName in cellNameComptDict:
                self.layoutWidget.drawNewCell(cellName)
            self.layoutWidget.updateGL()
            self.layoutWidget.setColorMap()
        elif self.modelHasIntFires:
            print 'modelHasIntFires:', len(self.allIntFires)
            sideSquare = self.nearestSquare(len(self.allIntFires))
            #for intFire in self.allIntFires:
            intFireCellNumber = 1 
            for yAxis in range(sideSquare):
                for xAxis in range(sideSquare):
                    self.layoutWidget.drawNewCell(cellName='/hopfield/cell_'+str(intFireCellNumber), style=3, cellCentre=[xAxis*0.5,yAxis*0.5,0.0])
                    intFireCellNumber += 1
            self.layoutWidget.updateGL()
            self.layoutWidget.setColorMap(vizMinVal = float(-1e-7), vizMaxVal = 0)
#setColorMap(self,vizMinVal=-0.1,vizMaxVal=0.07,moosepath='',variable='Vm',cMap='jet')
                
    def nearestSquare(self, n):	#add_chait
    	i = 1
	while i * i < n:
            i += 1
	return i

    def getElementpath(self,text):
        #print "here",text
        if(text == '/' or text == ''):
            QtGui.QMessageBox.about(self,"My message box","Target Element path should not be \'/\' or \'null\', filename will be selected as Element path")


    def resizeEvent(self, event):
        QtGui.QWidget.resizeEvent(self, event)

    def addKKITLayoutWindow(self,modelpath):
        centralWindowsize =  self.layoutWidget.size()
        layout = QtGui.QHBoxLayout(self.layoutWidget)
        self.sceneLayout = kineticlayout.kineticsWidget(centralWindowsize,modelpath,self.layoutWidget)
        self.sceneLayout.setSizePolicy(QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding))
        self.connect(self.sceneLayout, QtCore.SIGNAL("itemDoubleClicked(PyQt_PyObject)"), self.makeObjectFieldEditor)
        layout.addWidget(self.sceneLayout)
        self.layoutWidget.setLayout(layout)
        self.sceneLayout.show()

        #property editor dock related
    def refreshObjectEditor(self,item,number):
        self.makeObjectFieldEditor(item.getMooseObject())

    def makeObjectFieldEditor(self, obj):
        if obj.class_ == 'Shell' or obj.class_ == 'PyMooseContext' or obj.class_ == 'GenesisParser':
            print '%s of class %s is a system object and not to be edited in object editor.' % (obj.path, obj.class_)
            return
        self.propEditorCurrentSelection = obj
        try:
            self.objFieldEditModel = self.objFieldEditorMap[obj.getId()]
        except KeyError:
            self.objFieldEditModel = ObjectFieldsModel(obj)
            self.objFieldEditorMap[obj.getId()] = self.objFieldEditModel

        self.mTable.setObjectName(str(obj.getId()))
        self.mTable.setModel(self.objFieldEditModel)
        if hasattr(self, 'sceneLayout'):
            self.connect(self.objFieldEditModel,QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'),self.sceneLayout.updateItemSlot)
        self.updatePlotDockFields(obj)
        self.propEditorChildren(obj)

    def propEditorSelectParent(self):
        if self.propEditorCurrentSelection != None:
            self.makeObjectFieldEditor(self.propEditorCurrentSelection.getField('parent'))

    def propEditorSelectChild(self,item):
        formattedText = str(item.text()).lstrip('    ')
        self.makeObjectFieldEditor(moose.Neutral(self.propEditorChildrenIdDict[formattedText]))
        
    def propEditorChildren(self,obj):
        allChildren = obj.getField('children')
        self.propEditorChildrenIdDict = {}
        self.propEditorChildListWidget.clear()

        self.propEditorChildListWidget.addItem(obj.getField('name'))
        self.propEditorChildrenIdDict[obj.getField('name')] = obj.getId()

        for child in allChildren:
            self.propEditorChildrenIdDict[moose.Neutral(child).getField('name')] = child
            self.propEditorChildListWidget.addItem('    '+moose.Neutral(child).getField('name'))

        #plot config dock related 
    def updatePlots(self,currentTime):
        #updates plots every update_plot_dt time steps see moosehandler.
        for plotWinName,plotWin in self.plotNameWinDict.iteritems():
            plotWin.plot.updatePlot(currentTime)
        self.updateCurrentTime(currentTime)
        if self.modelHasCompartments or self.modelHasIntFires:
            self.updateVisualization()

    def updatePlotDockFields(self,obj):
        #add plot-able elements according to predefined  
        self.plotConfigCurrentSelectionLabel.setText(obj.getField('name'))
        fieldType = obj.getField('class')
        self.plotConfigCurrentSelectionTypeLabel.setText(fieldType)
        self.plotConfigFieldSelectionComboBox.clear()
        try: 
            self.plotConfigFieldSelectionComboBox.addItems(PLOT_FIELDS[fieldType])
            self.plotConfigCurrentSelection = obj
            self.plotConfigAcceptPushButton.setEnabled(True)
        except KeyError:
            #undefined field - see PLOT_FIELDS variable in defaults.py
            self.plotConfigFieldSelectionComboBox.clear()
            self.plotConfigCurrentSelection = None
            self.plotConfigAcceptPushButton.setEnabled(False)

    def populateDataPlots(self,modelpath):
        """Create plots for all Table objects in /data element"""

    def addFieldToPlot(self):
        #creates tables - called when 'Okay' pressed in plotconfig dock
        dataNeutral = moose.Neutral(self.plotConfigCurrentSelection.getField('path')+'/data')
        newTable = moose.Table(self.plotConfigCurrentSelection.getField('path')+'/data/'+str(self.plotConfigFieldSelectionComboBox.currentText()))
        moose.connect(newTable,'requestData', self.plotConfigCurrentSelection,'get_'+str(self.plotConfigFieldSelectionComboBox.currentText()))
        moose.useClock(4, newTable.getField('path'), 'process') #assign clock after creation itself 

        if str(self.plotConfigWinSelectionComboBox.currentText()) in self.plotWindowFieldTableDict:
            #case when plotwin already exists - append new table to mooseplotwin
            self.plotWindowFieldTableDict[str(self.plotConfigWinSelectionComboBox.currentText())].append(newTable)
            #select the corresponding plot (mooseplot) from the plotwindow (mooseplotwindow) 
            plotWin = self.plotNameWinDict[str(self.plotConfigWinSelectionComboBox.currentText())] 
            
            #do not like the legends shown in the plots, change the field 2 below
            plotWin.plot.addTable(newTable,newTable.getField('name'))
            
        else:
            #no previous mooseplotwin - so create, and add table to corresp dict
            self.plotWindowFieldTableDict[str(self.plotConfigWinSelectionComboBox.currentText())] = [newTable]
            plotWin = MoosePlotWindow(self)
            plotWin.setWindowTitle(str(self.plotConfigWinSelectionComboBox.currentText()))

            #do not like the legends shown in the plots, change the field 2 below
            plotWin.plot.addTable(newTable,newTable.getField('name'))
            self.plotNameWinDict[str(self.plotConfigWinSelectionComboBox.currentText())] = plotWin
            plotWin.show()

    def plotConfigAddNewPlotWindow(self):
        #called when new plotwindow pressed in plotconfig dock
        count = self.plotConfigWinSelectionComboBox.count()
        self.plotConfigWinSelectionComboBox.addItem('Plot Window '+str(count+1))
        self.plotConfigWinSelectionComboBox.setCurrentIndex(count)
        
        #general
    def updateCurrentTime(self,currentTime):
        self.currentTime = currentTime
        self.simControlCurrentTimeLabel.setText(str(self.currentTime))

    def doQuit(self):
        QtGui.qApp.closeAllWindows()

    def _resetAndRunSlot(self): #called when run is pressed
        try:
            runtime = float(str(self.simControlRunTimeLineEdit.text()))
        except ValueError:
            runtime = MooseHandler.runtime
            self.simControlRunTimeLineEdit.setText(str(runtime))
        self.mooseHandler.doResetAndRun(runtime,float(self.simControlSimdtLineEdit.text()),float(self.simControlPlotdtLineEdit.text()),float(self.simControlUpdatePlotdtLineEdit.text()))
        self.simControlSimdtLineEdit.setEnabled(False)
        self.simControlPlotdtLineEdit.setEnabled(False)
        self.simControlUpdatePlotdtLineEdit.setEnabled(False)

    def _resetSlot(self): #called when reset is pressed
        try:
            runtime = float(str(self.simControlRunTimeLineEdit.text()))
        except ValueError:
            runtime = MooseHandler.runtime
            self.simControlRunTimeLineEdit.setText(str(runtime))
        self.mooseHandler.doReset(float(self.simControlSimdtLineEdit.text()),float(self.simControlPlotdtLineEdit.text()))
        self.updatePlots(self.mooseHandler.getCurrentTime()) #clears the plots
        self.simControlSimdtLineEdit.setEnabled(True)
        self.simControlPlotdtLineEdit.setEnabled(True)
        self.simControlUpdatePlotdtLineEdit.setEnabled(True)

    def _continueSlot(self): #called when continue is pressed
        try:
            runtime = float(str(self.simControlRunTimeLineEdit.text()))
        except ValueError:
            runtime = MooseHandler.runtime
            self.simControlRunTimeLineEdit.setText(str(runtime))
        self.mooseHandler.doRun(float(str(self.simControlRunTimeLineEdit.text()))) 
        self.updatePlots(self.mooseHandler.getCurrentTime()) #updates the plots
        
    def updateDefaultTimes(self, modeltype): 
        if(modeltype == MooseHandler.type_kkit):
            self.mooseHandler.updateDefaultsKKIT()

        self.simControlSimdtLineEdit.setText(str(self.mooseHandler.simdt))
        self.simControlPlotdtLineEdit.setText(str(self.mooseHandler.plotdt))
        self.simControlUpdatePlotdtLineEdit.setText(str(self.mooseHandler.plotupdate_dt)) 
        self.simControlRunTimeLineEdit.setText(str(self.mooseHandler.runtime)) #default run time taken
            
        '''
        if (modeltype == MooseHandler.type_kkit) or (modeltype == MooseHandler.type_sbml):
            self.simdtText.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_SIMDT_KKIT)))
            self.plotdtText.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_PLOTDT_KKIT)))
            #self.gldtText.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_GLDT_KKIT)))
            self.runtimeText.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_RUNTIME_KKIT)))
            #harsha run from menu takes from self.runtimeText and run for toolbar takes from self.runTimeEditToolbar
            self.runTimeEditToolbar.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_RUNTIME_KKIT)))
            self.updateTimeText.setText(QtCore.QString('%1.3e' % (MooseHandler.DEFAULT_PLOTUPDATE_DT_KKIT)))
            '''
    def populateKKitPlots(self,path):
        graphs = self.getKKitGraphs(path) #clocks are assigned in the script itself!
        #currently putting all plots on Plot Window 1 by default - perhaps not the nicest way to do it.
        self.plotWindowFieldTableDict['Plot Window 1'] = graphs
        plotWin = MoosePlotWindow(self)
        plotWin.setWindowTitle('Plot Window 1')
        for graph in graphs:
            plotWin.plot.addTable(graph,graph.getField('name'))
        plotWin.show()
        self.plotNameWinDict['Plot Window 1'] = plotWin

    def getKKitGraphs(self,path):
        tableList = []
        for child in moose.wildcardFind(path+'/graphs/#/##[TYPE=Table],'+path+'/moregraphs/#/##[TYPE=Table]'):
            tableList.append(moose.Table(child))
        return tableList

# create the GUI application
app = QtGui.QApplication(sys.argv)
# instantiate the main window
dmw = DesignerMainWindow()
dmw.setWindowState(Qt.WindowMaximized)
# show it
dmw.show()
# start the Qt main loop execution, exiting from this script
#http://code.google.com/p/subplot/source/browse/branches/mzViewer/PyMZViewer/mpl_custom_widget.py
#http://eli.thegreenplace.net/files/prog_code/qt_mpl_bars.py.txt
#http://lionel.textmalaysia.com/a-simple-tutorial-on-gui-programming-using-qt-designer-with-pyqt4.html
#http://www.mail-archive.com/matplotlib-users@lists.sourceforge.net/msg13241.html
# with the same return code of Qt application
sys.exit(app.exec_())

