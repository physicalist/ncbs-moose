# used to parse files more easily
from __future__ import with_statement
# Numpy module
import numpy as np
# for command-line arguments
import sys
#moose imports
import moose
from collections import defaultdict
from objectedit import ObjectFieldsModel, ObjectEditDelegate, ObjectEditView
# Qt4 bindings for core Qt functionalities (non-GUI)
from PyQt4 import QtCore
from PyQt4.QtCore import QEvent, Qt
# Python Qt4 bindings for GUI objects
from PyQt4 import QtGui
# import the MainWindow widget from the converted .ui files
from MooseLayout import Ui_MainWindow
#opengl imports
from PyQt4.QtOpenGL import QGLWidget
from OpenGL.GL import *
from OpenGL.GLUT import *
from updatepaintGL import newGLSubWindow,updatepaintGL,newGLWindow
from morphml_writer import morphml_writer
mc=moose.context

class DesignerMainWindow(QtGui.QMainWindow, Ui_MainWindow):
    """Customization for Qt Designer created window"""
    def __init__(self, parent = None):
        # initialization of the superclass
        super(DesignerMainWindow, self).__init__(parent)
        # setup the GUI --> function generated by pyuic4
        self.setupUi(self)

	self.glPlotNumber = 0
	self.objFieldEditorMap = {}

	QtCore.QObject.connect(self.toolButton, QtCore.SIGNAL("clicked()"), self.update_graph) 
        QtCore.QObject.connect(self.toolButton_3, QtCore.SIGNAL("clicked()"), self.writeXML) 
	QtCore.QObject.connect(self.toolButton_2, QtCore.SIGNAL("clicked()"), self.newGLWindow) 
	QtCore.QObject.connect(self.mtree,QtCore.SIGNAL('itemDoubleClicked(QTreeWidgetItem *, int)'),self.makeObjEditorFromTreeItem)
	QtCore.QObject.connect(self.qgl,QtCore.SIGNAL("compartmentSelected(QString)"),self.pickCompartment)
	

    def writeXML(self):
        xmlExport = morphml_writer("testing the writer","meter")
        an=moose.Neutral('/')
	all_ch=an.childList 					#all children
	ch = self.get_childrenOfField(all_ch,'Cell')	
	for i in range(0,len(ch),1):
            nameOfCell = mc.getField(ch[i],'name')
            cell_xml = xmlExport.set_cell(nameOfCell)

            an = moose.Neutral(ch[i])
            all_cp = an.childList
            cp = self.get_childrenOfField(all_cp,'Compartment')	
            for j in range(0,len(cp)):
    	    	x=str(mc.getField(cp[j],'x'))
    	    	y=str(mc.getField(cp[j],'y'))
    	    	z=str(mc.getField(cp[j],'z'))
    	    	x0=str(mc.getField(cp[j],'x0'))
    	    	y0=str(mc.getField(cp[j],'y0'))
	   	z0=str(mc.getField(cp[j],'z0'))
	   	d=str(mc.getField(cp[j],'diameter'))
                xmlExport.set_segment(cell_xml,str(j),mc.getField(cp[j],'name'),x0,y0,z0,x,y,z,d)

        xmlExport.writeToFile("this.xml")

    def newGLWindow(self):
    	self.glPlotNumber += 1
        qgl2 = newGLWindow(parent=self)
        qgl2.windowTitle('GL Window ' + str(self.glPlotNumber))
	qgl2.show()
	qgl2.raise_()

    def pickCompartment(self,path):	#path is a QString type moosepath
        SelectedChild = self.mtree.pathToTreeChild(path)
    	self.mtree.setCurrentItem(SelectedChild)				#select the corresponding moosetree
	self.makeObjectFieldEditor(SelectedChild.getMooseObject())		#update the corresponding property

    def mooseCell(self):
        a = moose.Compartment('compartment1')
        a.x0 = 0
        a.y0 = 0
        a.z0 = 0
        a.x = 0
        a.y = 0
        a.z = 32e-6
        a.diameter = 16e-6
        a.length = 32e-6

    def update_graph(self):
        
        #self.mooseCell()

	#mc.loadG('Mitral.g')
	mc.readCell('Purkinje4M9.p','cell')
	#mc.readCell('/mit.p','cell2')
	self.mtree.recreateTree()			#refreshes the widget with the new elements.

	self.qgl.selectionMode=0			#selection mode, select cells or select compartments. 
	
	#self.qgl.viz=1								#turn on visualization, use with self.qgl.updateViz()	
	#self.qgl.setColorMap(vizMinVal=120*1e+06,vizMaxVal=45000*1e+06,cMap='jet')		#set the color map for visualization
	
	#self.qgl.drawAllCells(2)					#draws all cells in the moose root, in the specified style
	
	#self.qgl.drawAllCellsUnder('/shell',2)				#draws cells in the given path, path must be a neutral element
	
	#self.qgl.gridCompartmentName ='axon'
	self.qgl.drawNewCell('/cell',style=2,cellCentre=[0,0,0])	
	
	#self.qgl.drawNewCell('/cell',style=2,cellCentre=[0,0,0])	#draws the canvas with just the given cellpath
	
	#self.qgl.updateViz()						#update the visualization, call once every t frames
	
	#self.qgl.viz=0
	
	#self.qgl.drawNewCell('/cell2',style=1,cellCentre=[1,1,1])
	
	#self.qgl.updateViz()	
	#self.qgl.saveVizAll('tada.pkl')
	self.qgl.updateGL()	

	
    # def cmp_position(self,line_label): 		#used with onpickplaceline, returns the absolute value of the end of compartment position
    #     a_id=mc.pathToId(line_label)
    #     x= float(mc.getField(a_id,'x'))
    #     y= float(mc.getField(a_id,'y'))
    #     z= float(mc.getField(a_id,'z'))	
    #     return x,y,z


    def makeObjEditorFromTreeItem(self, item, column):
        """Wraps makeObjectFieldEditor for use via a tree item"""
        obj = item.getMooseObject()
        self.makeObjectFieldEditor(obj)	
        if item.mooseObj_.className == 'Compartment':
        	self.qgl.selectedObjects.removeAll()
        	iv = self.qgl.sceneObjectNames.index(obj.path)
        	self.qgl.selectedObjects.add(self.qgl.sceneObjects[iv])
		self.qgl.updateGL()
		
	elif item.mooseObj_.className=='Cell':
		self.qgl.selectedObjects.removeAll()
		for cmpt in self.qgl.sceneObjects:
			if cmpt.daddy==item.mooseObj_.path:
        			self.qgl.selectedObjects.add(cmpt)
        	self.qgl.updateGL()
				
    def get_childrenOfField(self,all_ch,field):	#'all_ch' is a tuple of moose.id, 'field' is the field to sort with; returns a tuple with valid moose.id's
        ch=[]
        for i in range(0,len(all_ch)):	
	    if(mc.className(all_ch[i])==field):
	        ch.append(all_ch[i])
        return tuple(ch)  
    
    def makeObjectFieldEditor(self, obj):
        """Creates a table-editor for a selected object."""
        try:
            self.objFieldEditModel = self.objFieldEditorMap[obj.id]
        except KeyError:
            self.objFieldEditModel = ObjectFieldsModel(obj)
            self.objFieldEditorMap[obj.id] = self.objFieldEditModel
            #self.connect(self.objFieldEditModel, QtCore.SIGNAL('plotWindowChanged(const QString&, const QString&)'), self.changeFieldPlotWidget)
           
	    #if  not hasattr(self, 'objFieldEditPanel'):
            self.mtable.setObjectName(self.tr('MooseObjectFieldEdit'))

            #self.connect(self.objFieldEditModel, QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'), self.renameObjFieldEditPanel)
            
                
        #self.objFieldEditor = ObjectEditView(self.mtable)
        self.mtable.setObjectName(str(obj.id)) # Assuming Id will not change in the lifetime of the object - something that might break in future version.
        self.mtable.setModel(self.objFieldEditModel)
        self.mtable.setEditTriggers(QtGui.QAbstractItemView.DoubleClicked
                                 | QtGui.QAbstractItemView.SelectedClicked)
        self.mtable.setDragEnabled(True)
        #for plot in self.plots:
        #    objName = plot.objectName()
        #    if objName not in self.objFieldEditModel.plotNames :
        #        self.objFieldEditModel.plotNames += [plot.objectName() for plot in self.plots]
        self.mtable.setItemDelegate(ObjectEditDelegate(self))
        self.connect(self.objFieldEditModel, 
                     QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'),
                     self.mtree.updateItemSlot)
	if hasattr(self, 'sceneLayout'):
	        self.connect(self.objFieldEditModel, 
        	             QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'),
        	             self.sceneLayout.updateItemSlot)

        #self.mtable.setWidget(self.objFieldEditor)
        self.mtable.setWindowTitle(self.tr(obj.name))
	self.mtable.show()


#print 'tell you what'
#glutInit(sys.argc,sys.argv)	
# create the GUI application
app = QtGui.QApplication(sys.argv)

# instantiate the main window
dmw = DesignerMainWindow()
# show it
dmw.show()
# start the Qt main loop execution, exiting from this script
#http://code.google.com/p/subplot/source/browse/branches/mzViewer/PyMZViewer/mpl_custom_widget.py
#http://eli.thegreenplace.net/files/prog_code/qt_mpl_bars.py.txt
#http://lionel.textmalaysia.com/a-simple-tutorial-on-gui-programming-using-qt-designer-with-pyqt4.html
#http://www.mail-archive.com/matplotlib-users@lists.sourceforge.net/msg13241.html
# with the same return code of Qt application
sys.exit(app.exec_())
