# squid_demo.py --- 
# 
# Filename: squid_demo.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Feb 22 23:24:21 2012 (+0530)
# Version: 
# Last-Updated: Thu Mar  8 20:23:00 2012 (+0530)
#           By: Subhasis Ray
#     Update #: 849
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 

# Code:
import sys
from threading import Thread, Timer
from Queue import Queue, Empty
import time

from PyQt4 import QtGui
from PyQt4 import QtCore
from PyQt4 import Qwt5 as Qwt
import PyQt4.Qwt5.anynumpy as numpy
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas, NavigationToolbar2QTAgg as NavigationToolbar

import moose

from squid import SquidAxon
from squid_setup import SquidSetup
from electronics import ClampCircuit

class SimulationThread(QtCore.QThread):
    def __init__(self, parent=None):
        QtCore.QThread.__init__(self, parent)
        self.exiting = False
        self.runtime = 0
        self.mutex = QtCore.QMutex()
        
    def exit(self):
        try:
            self.mutex.lock()
            self.exiting = True
        finally:
            self.mutex.unlock()

    def set_runtime(self, runtime):
        try:
            self.mutex.lock()
            self.runtime = runtime
        finally:
            self.mutex.unlock()
            
    def run(self):
        while not self.exiting:
            if not moose.isRunning() and self.runtime > 0:
                try:
                    self.mutex.lock()
                    print 'SimulationThread::Starting simulation'
                    moose.start(self.runtime)
                    self.runtime = 0
                finally:
                    self.mutex.unlock()
            self.msleep(300)
            
    def __del__(self):
        self.wait()

# class SimulationThread(Thread):
#     """This thread runs the simulation and controls data exchange
#     between Qt/matplotlib and MOOSE."""
#     def __init__(self, runtime_queue):
#         """runtime_queue is a queue which can be filled by a client
#         with runtime for the next simulation run. If runtime is zero
#         or negative, the thread's run method returns."""
#         Thread.__init__(self)
#         self.runtime_queue = runtime_queue
        
#     def run(self):
#         while True:
#             if not moose.isRunning():
#                 try:
#                     runtime = self.runtime_queue.get(block=True)
#                     if runtime <= 0:
#                         return
#                     print 'SimulationThread: starting for', runtime
#                     moose.start(runtime)
#                 except Empty:
#                     print 'SimulationThread: queue empty'
#                     pass
#             time.sleep(3.0) # humans take offence at delays beyond 3 seconds
    
class SquidDemo(QtGui.QMainWindow):
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.squid_setup = SquidSetup()
        self.setWindowTitle('Squid Axon Demo')
        self.setCentralWidget(self._getMdiArea())
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._getRunControl())
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._getElectronicsCtrl())
        self._initActions()
        self._getToolBar()
        # self._runtime_queue = Queue()
        self._timer = None
        self._simthread = SimulationThread()
        self._simthread.start()
        # self._simthread = SimulationThread(self._runtime_queue)
        # self._simthread.start()
        
    def _getMdiArea(self):
        if hasattr(self, '_mdiArea'):
            return self._mdiArea
        self._mdiArea = QtGui.QMdiArea(self)
        self._plotSubWindow = self._mdiArea.addSubWindow(self._getPlotWidget())
        self._plotSubWindow.setWindowTitle('Plots')
        self._statePlotSubWindow = self._mdiArea.addSubWindow(self._getStatePlotWidget())
        self._statePlotSubWindow.setWindowTitle('State plot')
        self._statePlotSubWindow.hide()
        
        return self._mdiArea

    def _getRunControl(self):
        if hasattr(self, '_runControlWidget'):
            return self._runControlWidget
        self._runControlWidget = QtGui.QDockWidget(self)
        self._simInputBox = QtGui.QGroupBox()
        self._runTimeLabel = QtGui.QLabel("Run time (ms)", self._runControlWidget)
        self._simTimeStepLabel = QtGui.QLabel("Simulation time step (ms)", self._runControlWidget)
        self._plotTimeStepLabel = QtGui.QLabel("Plotting interval (ms)", self._runControlWidget)
        self._runTimeEdit = QtGui.QLineEdit("50.0", self._runControlWidget)
        self._simTimeStepEdit = QtGui.QLineEdit("0.01", self._runControlWidget)
        self._plotTimeStepEdit = QtGui.QLineEdit("0.1", self._runControlWidget)
        layout = QtGui.QGridLayout()
        layout.addWidget(self._runTimeLabel, 0,0)
        layout.addWidget(self._runTimeEdit, 0, 1)
        layout.addWidget(self._simTimeStepLabel, 1, 0)
        layout.addWidget(self._simTimeStepEdit, 1, 1)
        layout.addWidget(self._plotTimeStepLabel, 2, 0)
        layout.addWidget(self._plotTimeStepEdit, 2, 1)
        self._simInputBox.setLayout(layout)
        self._runControlWidget.setWidget(self._simInputBox)
        return self._runControlWidget

    def _getPlotWidget(self):
        if hasattr(self, '_plotWidget'):
            return self._plotWidget
        self._plotWidget = QtGui.QWidget()
        self._plotFigure = Figure()
        self._plotCanvas = FigureCanvas(self._plotFigure)
        self._plotCanvas.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._plotCanvas.updateGeometry()
        self._plotCanvas.setParent(self._plotWidget)
        self._plotFigure.set_canvas(self._plotCanvas)
        # Vm and command voltage go in the same subplot
        self._vm_axes = self._plotFigure.add_subplot(2,2,1, title='Membrane potential')
        self._vm_axes.set_ylim(-20.0, 120.0)
        self._vm_plot, = self._vm_axes.plot([], [], label='Vm')
        self._command_plot, = self._vm_axes.plot([], [], label='command')
        self._vm_axes.legend()
        # Channel conductances go to the same subplot
        self._g_axes = self._plotFigure.add_subplot(2,2,2, title='Channel conductance')
        self._g_axes.set_ylim(0.0, 0.5)
        self._gna_plot, = self._g_axes.plot([], [], label='Na')
        self._gk_plot, = self._g_axes.plot([], [], label='K')
        self._g_axes.legend()
        # Injection current for Vclamp/Iclamp go to the same subplot
        self._im_axes = self._plotFigure.add_subplot(2,2,3, title='Injection current')
        self._im_axes.set_ylim(-0.5, 0.5)
        self._iclamp_plot, = self._im_axes.plot([], [], label='Iclamp')
        self._vclamp_plot, = self._im_axes.plot([], [], label='Vclamp')
        self._im_axes.legend()
        # Channel currents go to the same subplot
        self._i_axes = self._plotFigure.add_subplot(2,2,4, title='Channel current')
        self._i_axes.set_ylim(-10, 10)
        self._ina_plot, = self._i_axes.plot([], [], label='Na')
        self._ik_plot, = self._i_axes.plot([], [], label='K')
        self._i_axes.legend()
        self._plotNavigator = NavigationToolbar(self._plotCanvas, self._plotWidget)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self._plotCanvas)
        layout.addWidget(self._plotNavigator)
        self._plotWidget.setLayout(layout)
        return self._plotWidget

    def _getStatePlotWidget(self):
        if hasattr(self, '_statePlotWidget'):
            return self._statePlotWidget
        self._statePlotWidget = QtGui.QWidget()
        self._statePlotFigure = Figure()
        self._statePlotCanvas = FigureCanvas(self._statePlotFigure)
        self._statePlotCanvas.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._statePlotCanvas.updateGeometry()
        self._statePlotCanvas.setParent(self._statePlotWidget)
        self._statePlotFigure.set_canvas(self._statePlotCanvas)
        self._statePlotFigure.subplots_adjust(hspace=0.5)
        self._statePlotAxes = self._statePlotFigure.add_subplot(2,1,1, title='State plot')
        self._state_plot, = self._statePlotAxes.plot([], [], label='state')
        self._statePlotAxes.legend()
        self._activationParamAxes = self._statePlotFigure.add_subplot(2,1,2, title='H-H activation parameters vs time')
        self._activationParamAxes.set_xlabel('Time (ms)')
        self._m_plot, = self._activationParamAxes.plot([],[], label='m')
        self._h_plot, = self._activationParamAxes.plot([], [], label='h')
        self._n_plot, = self._activationParamAxes.plot([], [], label='n')
        self._activationParamAxes.legend()
        self._stateplot_xvar_label = QtGui.QLabel('Variable on X-axis')
        self._stateplot_xvar_combo = QtGui.QComboBox()
        self._stateplot_xvar_combo.addItems(['V', 'm', 'n', 'h'])
        self._stateplot_xvar_combo.setCurrentIndex(0)
        self._stateplot_xvar_combo.setEditable(False)
        self.connect(self._stateplot_xvar_combo,
                     QtCore.SIGNAL('currentIndexChanged(const QString&)'),
                     self._statePlotXSlot)                     
        self._stateplot_yvar_label = QtGui.QLabel('Variable on Y-axis')
        self._stateplot_yvar_combo = QtGui.QComboBox()
        self._stateplot_yvar_combo.addItems(['V', 'm', 'n', 'h'])
        self._stateplot_yvar_combo.setCurrentIndex(2)
        self._stateplot_yvar_combo.setEditable(False)
        self.connect(self._stateplot_yvar_combo,
                     QtCore.SIGNAL('currentIndexChanged(const QString&)'),
                     self._statePlotYSlot)
        self._statePlotNavigator = NavigationToolbar(self._statePlotCanvas, self._statePlotWidget)        
        frame = QtGui.QFrame()
        layout = QtGui.QHBoxLayout()
        layout.addWidget(self._stateplot_xvar_label)
        layout.addWidget(self._stateplot_xvar_combo)
        layout.addWidget(self._stateplot_yvar_label)
        layout.addWidget(self._stateplot_yvar_combo)
        frame.setLayout(layout)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(frame)
        layout.addWidget(self._statePlotCanvas)
        layout.addWidget(self._statePlotNavigator)
        self._statePlotWidget.setLayout(layout)
        return self._statePlotWidget
        
    def _initActions(self):
        self._runAction = QtGui.QAction(self.tr('Run'), self)
        self.connect(self._runAction, QtCore.SIGNAL('triggered()'), self._runSlot)
        self._showStatePlotAction = QtGui.QAction(self.tr('Show state plot'), self)
        self._showStatePlotAction.setCheckable(True)
        self._showStatePlotAction.setChecked(False)
        self.connect(self._showStatePlotAction, QtCore.SIGNAL('toggled(bool)'), self._statePlotSubWindow.setVisible)
        self._quitAction = QtGui.QAction(self.tr('&Quit'), self)
        self.connect(self._quitAction, QtCore.SIGNAL('triggered()'), self._quitSlot)

    def _quitSlot(self):
        print 'Quitting'
        self._simthread.exit()
        if self._timer:
            self.killTimer(self._timer)
        # self._runtime_queue.put(-1.0)
        # self._runtime_queue.join()
        QtGui.qApp.quit()
        print 'Finished'
        
    def _getToolBar(self):
        if hasattr(self, '_simToolBar'):
            return self._simToolBar
        self._simToolBar = self.addToolBar(self.tr('Simulation control'))
        self._simToolBar.addAction(self._quitAction)
        self._simToolBar.addAction(self._runAction)
        self._simToolBar.addAction(self._showStatePlotAction)

    def _getElectronicsCtrl(self):
        """Creates a tabbed widget of voltage clamp and current clamp controls"""
        if hasattr(self, '_electronicsCtrl'):
            return self._electronicsCtrl        
        self._electronicsCtrl = QtGui.QDockWidget(self)
        self._electronicsTab = QtGui.QTabWidget(self._electronicsCtrl)
        self._electronicsTab.addTab(self._getIClampCtrlBox(), 'Current clamp')
        self._electronicsTab.addTab(self._getVClampCtrlBox(), 'Voltage clamp')
        self._electronicsCtrl.setWidget(self._electronicsTab)
        return self._electronicsCtrl

    def _getVClampCtrlBox(self):
        if hasattr(self, '_vClampCtrlBox'):
            return self._vClampCtrlBox
        vClampPanel = QtGui.QGroupBox("Voltage-Clamp Settings", self)
        self._vClampCtrlBox = vClampPanel
        self._holdingVLabel = QtGui.QLabel("Holding Voltage (mV)", vClampPanel)
        self._holdingVEdit = QtGui.QLineEdit("0.0", vClampPanel)
        self._holdingTimeLabel = QtGui.QLabel("Holding Time (ms)", vClampPanel)
        self._holdingTimeEdit = QtGui.QLineEdit("10.0", vClampPanel)
        self._prePulseVLabel = QtGui.QLabel("Pre-pulse Voltage (mV)", vClampPanel)
        self._prePulseVEdit = QtGui.QLineEdit("0.0", vClampPanel)
        self._prePulseTimeLabel = QtGui.QLabel("Pre-pulse Time (ms)", vClampPanel)
        self._prePulseTimeEdit = QtGui.QLineEdit("0.0", vClampPanel)
        self._clampVLabel = QtGui.QLabel("Clamp Voltage (mV)", vClampPanel)
        self._clampVEdit = QtGui.QLineEdit("50.0", vClampPanel)
        self._clampTimeLabel = QtGui.QLabel("Clamp Time (ms)", vClampPanel)
        self._clampTimeEdit = QtGui.QLineEdit("20.0", vClampPanel)
        layout = QtGui.QGridLayout(vClampPanel)
        layout.addWidget(self._holdingVLabel, 0, 0)
        layout.addWidget(self._holdingVEdit, 0, 1)
        layout.addWidget(self._holdingTimeLabel, 1, 0)
        layout.addWidget(self._holdingTimeEdit, 1, 1)
        layout.addWidget(self._prePulseVLabel, 2, 0)
        layout.addWidget(self._prePulseVEdit, 2, 1)
        layout.addWidget(self._prePulseTimeLabel,3,0)
        layout.addWidget(self._prePulseTimeEdit, 3, 1)
        layout.addWidget(self._clampVLabel, 4, 0)
        layout.addWidget(self._clampVEdit, 4, 1)
        layout.addWidget(self._clampTimeLabel, 5, 0)
        layout.addWidget(self._clampTimeEdit, 5, 1)
        vClampPanel.setLayout(layout)
        return self._vClampCtrlBox

    
    def _getIClampCtrlBox(self):
        if hasattr(self, '_iClampCtrlBox'):
            return self._iClampCtrlBox
        iClampPanel = QtGui.QGroupBox("Current-Clamp Settings", self)
        self._iClampCtrlBox = iClampPanel
        self._baseCurrentLabel = QtGui.QLabel("Base Current Level (uA)",iClampPanel)
        self._baseCurrentEdit = QtGui.QLineEdit("0.0",iClampPanel)
        self._firstPulseLabel = QtGui.QLabel("First Pulse Current (uA)", iClampPanel)
        self._firstPulseEdit = QtGui.QLineEdit("0.1", iClampPanel)
        self._firstDelayLabel = QtGui.QLabel("First Onset Delay (ms)", iClampPanel)
        self._firstDelayEdit = QtGui.QLineEdit("5.0",iClampPanel)
        self._firstPulseWidthLabel = QtGui.QLabel("First Pulse Width (ms)", iClampPanel)
        self._firstPulseWidthEdit = QtGui.QLineEdit("40.0", iClampPanel)
        self._secondPulseLabel = QtGui.QLabel("Second Pulse Current (uA)", iClampPanel)
        self._secondPulseEdit = QtGui.QLineEdit("0.0", iClampPanel)
        self._secondDelayLabel = QtGui.QLabel("Second Onset Delay (ms)", iClampPanel)
        self._secondDelayEdit = QtGui.QLineEdit("0.0",iClampPanel)
        self._secondPulseWidthLabel = QtGui.QLabel("Second Pulse Width (ms)", iClampPanel)
        self._secondPulseWidthEdit = QtGui.QLineEdit("0.0", iClampPanel)
        self._pulseMode = QtGui.QComboBox(iClampPanel)
        self._pulseMode.addItem("Single Pulse")
        self._pulseMode.addItem("Pulse Train")
        layout = QtGui.QGridLayout(iClampPanel)
        layout.addWidget(self._baseCurrentLabel, 0, 0)
        layout.addWidget(self._baseCurrentEdit, 0, 1)
        layout.addWidget(self._firstPulseLabel, 1, 0)
        layout.addWidget(self._firstPulseEdit, 1, 1)
        layout.addWidget(self._firstDelayLabel, 2, 0)
        layout.addWidget(self._firstDelayEdit, 2, 1)
        layout.addWidget(self._firstPulseWidthLabel, 3, 0)
        layout.addWidget(self._firstPulseWidthEdit, 3, 1)
        layout.addWidget(self._secondPulseLabel, 4, 0)
        layout.addWidget(self._secondPulseEdit, 4, 1)
        layout.addWidget(self._secondDelayLabel, 5, 0)
        layout.addWidget(self._secondDelayEdit, 5, 1)
        layout.addWidget(self._secondPulseWidthLabel, 6, 0)
        layout.addWidget(self._secondPulseWidthEdit, 6, 1)
        layout.addWidget(self._pulseMode, 7, 0, 1, 2)
        layout.setSizeConstraint(QtGui.QLayout.SetFixedSize)
        iClampPanel.setLayout(layout)
        return self._iClampCtrlBox

    def __get_stateplot_data(self, name):
        data = []
        if name == 'V':
            data = self.squid_setup.vm_table.vec
        elif name == 'm':
            data = self.squid_setup.m_table.vec
        elif name == 'h':
            data = self.squid_setup.h_table.vec
        elif name == 'n':
            data = self.squid_setup.n_table.vec
        else:
            raise ValueError('Unrecognized selection: %s' % (name))
        return numpy.asarray(data)
    
    def _statePlotYSlot(self, selectedItem):
        ydata = self.__get_stateplot_data(str(selectedItem))
        self._state_plot.set_ydata(ydata)
        self._statePlotAxes.set_ylabel(selectedItem)
        if str(selectedItem) == 'V':
            self._statePlotAxes.set_ylim(-20, 120)
        else:
            self._statePlotAxes.set_ylim(0, 1)
        self._statePlotCanvas.draw()
        
    def _statePlotXSlot(self, selectedItem):
        xdata = self.__get_stateplot_data(str(selectedItem))
        self._state_plot.set_xdata(xdata)
        self._statePlotAxes.set_xlabel(selectedItem)
        if str(selectedItem) == 'V':
            self._statePlotAxes.set_xlim(-20, 120)
        else:
            self._statePlotAxes.set_xlim(0, 1)
        self._statePlotCanvas.draw()
        
    def _runSlot(self):
        self._simdt = float(str(self._simTimeStepEdit.text()))
        self._plotdt = float(str(self._plotTimeStepEdit.text()))
        clampMode = None
        singlePulse = True
        if self._electronicsTab.currentWidget() == self._getVClampCtrlBox():
            clampMode = 'vclamp'
            baseLevel = float(str(self._holdingVEdit.text()))
            firstDelay = float(str(self._holdingTimeEdit.text()))
            firstWidth = float(str(self._prePulseTimeEdit.text()))
            firstLevel = float(str(self._prePulseVEdit.text()))
            secondDelay = firstWidth
            secondWidth = float(str(self._clampTimeEdit.text()))
            secondLevel = float(str(self._clampVEdit.text()))
            self._im_axes.set_ylim(-10.0, 10.0)
        else:
            clampMode = 'iclamp'
            baseLevel = float(str(self._baseCurrentEdit.text()))
            firstDelay = float(str(self._firstDelayEdit.text()))
            firstWidth = float(str(self._firstPulseWidthEdit.text()))
            firstLevel = float(str(self._firstPulseEdit.text()))
            secondDelay = float(str(self._secondDelayEdit.text()))
            secondLevel = float(str(self._secondPulseEdit.text()))
            secondWidth = float(str(self._secondPulseWidthEdit.text()))
            singlePulse = (self._pulseMode.currentIndex() == 0)
            self._im_axes.set_ylim(-0.4, 0.4)
        self.squid_setup.clamp_ckt.configure_pulses(baseLevel=baseLevel,
                                                    firstDelay=firstDelay,
                                                    firstWidth=firstWidth,
                                                    firstLevel=firstLevel,
                                                    secondDelay=secondDelay,
                                                    secondWidth=secondWidth,
                                                    secondLevel=secondLevel,
                                                    singlePulse=singlePulse)
        self.squid_setup.schedule(self._simdt, self._plotdt, clampMode)
        self._runtime = float(str(self._runTimeEdit.text()))
        # The following line is for use with Qthread
        print 'Starting simulation'
        self._simthread.set_runtime(self._runtime)
        self._timer = self.startTimer(300)
        # self._runtime_queue.put(self._runtime)
        # self._timer = Timer(3.0, self._updateAllPlots)
        # self._timer.start()
        # time.sleep(3.0)

    def _updateAllPlots(self):
        print '_updateAllPlots'
        self._updatePlots()
        self._updateStatePlot()
        # if hasattr(self, '_timer'):
        #     self._timer.cancel()
        # self._timer = Timer(3.0, self._updateAllPlots)        
        # self._timer.start()
        # time.sleep(3.0)
        
    def _updatePlots(self):
        if len(self.squid_setup.vm_table.vec) <= 0:
            return        
        self._vm_axes.set_xlim(0.0, self._runtime)
        self._g_axes.set_xlim(0.0, self._runtime)
        self._im_axes.set_xlim(0.0, self._runtime)
        self._i_axes.set_xlim(0.0, self._runtime)
        vm = numpy.asarray(self.squid_setup.vm_table.vec)
        print 'Size of vm', len(vm)
        cmd = numpy.asarray(self.squid_setup.cmd_table.vec)
        ik = numpy.asarray(self.squid_setup.ik_table.vec)
        ina = numpy.asarray(self.squid_setup.ina_table.vec)
        iclamp = numpy.asarray(self.squid_setup.iclamp_table.vec)
        vclamp = numpy.asarray(self.squid_setup.vclamp_table.vec)
        gk = numpy.asarray(self.squid_setup.gk_table.vec)
        gna = numpy.asarray(self.squid_setup.gna_table.vec)
        time_series = numpy.linspace(0, self._plotdt * len(vm), len(vm))        
        self._vm_plot.set_data(time_series, vm)
        time_series = numpy.linspace(0, self._plotdt * len(cmd), len(cmd))        
        self._command_plot.set_data(time_series, cmd)
        time_series = numpy.linspace(0, self._plotdt * len(ik), len(ik))
        self._ik_plot.set_data(time_series, ik)
        time_series = numpy.linspace(0, self._plotdt * len(ina), len(ina))
        self._ina_plot.set_data(time_series, ina)
        time_series = numpy.linspace(0, self._plotdt * len(iclamp), len(iclamp))
        self._iclamp_plot.set_data(time_series, iclamp)
        time_series = numpy.linspace(0, self._plotdt * len(vclamp), len(vclamp))
        self._vclamp_plot.set_data(time_series, vclamp)
        time_series = numpy.linspace(0, self._plotdt * len(gk), len(gk))
        self._gk_plot.set_data(time_series, gk)
        time_series = numpy.linspace(0, self._plotdt * len(gna), len(gna))
        self._gna_plot.set_data(time_series, gna)
        self._plotCanvas.draw()

    def _updateStatePlot(self):
        if len(self.squid_setup.vm_table.vec) <= 0:
            return
        sx = str(self._stateplot_xvar_combo.currentText())
        sy = str(self._stateplot_yvar_combo.currentText())
        xdata = self.__get_stateplot_data(sx)
        ydata = self.__get_stateplot_data(sy)
        minlen = min(len(xdata), len(ydata))
        self._state_plot.set_data(xdata[:minlen], ydata[:minlen])
        self._statePlotAxes.set_xlabel(sx)
        self._statePlotAxes.set_ylabel(sy)
        if sx == 'V':
            self._statePlotAxes.set_xlim(-20, 120)
        else:
            self._statePlotAxes.set_xlim(0, 1)
        if sy == 'V':
            self._statePlotAxes.set_ylim(-20, 120)
        else:
            self._statePlotAxes.set_ylim(0, 1)
        self._activationParamAxes.set_xlim(0, self._runtime)
        m = self.__get_stateplot_data('m')
        n = self.__get_stateplot_data('n')
        h = self.__get_stateplot_data('h')
        time_series = numpy.linspace(0, self._plotdt*len(m), len(m))
        self._m_plot.set_data(time_series, m)
        time_series = numpy.linspace(0, self._plotdt*len(h), len(h))
        self._h_plot.set_data(time_series, h)
        time_series = numpy.linspace(0, self._plotdt*len(n), len(n))
        self._n_plot.set_data(time_series, n)
        self._statePlotCanvas.draw()
                
    def timerEvent(self, event):
        print 'timerEvent '
        self._updateAllPlots()
        if not moose.isRunning():
            self.killTimer(event.timerId())

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    QtGui.qApp = app
    squid_gui = SquidDemo()
    app.connect(app, QtCore.SIGNAL('lastWindowClosed()'), squid_gui._quitSlot)
    squid_gui.show()
    sys.exit(app.exec_())
    

# 
# squid_demo.py ends here
