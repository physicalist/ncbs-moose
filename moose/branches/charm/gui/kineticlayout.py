import sys
import os
from PyQt4 import QtGui,QtCore,Qt
#import pygraphviz as pgv
import pickle
import random
import config
import re
import math
sys.path.append('../python')
from filepaths import *

from moose import *

class Rectcplx(QtGui.QGraphicsRectItem):
    def __init__(self,x,y,w,h,parent,item):
        self.Rectemitter1 = QtCore.QObject()
        self.mooseObj_ = item
        self.layoutWidgetPt = parent

        QtGui.QGraphicsRectItem.__init__(self,x,y,w,h,parent)

        #self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)

        #~ self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
        if config.QT_MINOR_VERSION >= 6:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 

    def pointerLayoutpt(self):
        return (self.layoutWidgetPt)
    
    def mouseDoubleClickEvent(self, event):
        self.Rectemitter1.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),element(self.mooseObj_))
    
    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.Rectemitter1.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),element(self.mooseObj_))
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.Rectemitter1.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),element(self.mooseObj_))
        return QtGui.QGraphicsItem.itemChange(self,change,value)

class Textitem(QtGui.QGraphicsSimpleTextItem):
    def __init__(self,parent,item,xpos,ypos,pickle):
        self.mooseObj_ = item[0][0]
        textcolor = item[3]
        bgcolor = item[4]
        self.picklecolorMap = pickle
        self.parent = parent
        self.textemitter = QtCore.QObject()
        if isinstance(parent,KineticsWidget):
            QtGui.QGraphicsSimpleTextItem.__init__(self,self.mooseObj_.name)
            self.layoutWidgetpt = parent
        elif isinstance(parent,ComptRect):
            self.layoutWidgetpt = parent.pointerLayoutpt()
            QtGui.QGraphicsSimpleTextItem.__init__(self, self.mooseObj_.name, self.parent)

        textcolor,b = self.layoutWidgetpt.colorCheck(textcolor,bgcolor,self.picklecolorMap)
        tcolor = Qt.QColor(textcolor)
        self.bcolor = Qt.QColor(b)
        self.setPen(QtGui.QPen(QtGui.QBrush(Qt.Qt.black)))
        self.setBrush(QtGui.QBrush(tcolor))
        self.setPos(xpos,ypos)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        
        if config.QT_MINOR_VERSION >= 6:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 

    def paint(self, painter, option, widget):
        painter.setPen(Qt.QColor(0,0,0,0))
        painter.setBrush(QtGui.QBrush(self.bcolor))
        painter.drawRect(self.boundingRect().x(),self.boundingRect().y(),self.boundingRect().width(),self.boundingRect().height())
        QtGui.QGraphicsSimpleTextItem.paint(self, painter, option, widget)

    def mouseDoubleClickEvent(self, event):
        self.textemitter.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),element(self.mooseObj_))

    
    def updateSlot(self):
        self.setText(self.mooseObj_.name)
    
    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.textemitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mooseObj_)
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.textemitter.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.mooseObj_)
        return QtGui.QGraphicsItem.itemChange(self,change,value)
       
class EllipseItem(QtGui.QGraphicsEllipseItem):
    positionChange = QtCore.pyqtSignal(QtGui.QGraphicsItem)
    selectedChange = QtCore.pyqtSignal(QtGui.QGraphicsItem)

    def __init__(self,x,y,w,h,parent,mooseObj):
        self.ellemitter = QtCore.QObject()
        self.mooseObj_ = mooseObj[0]
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.parent = parent
        if isinstance(parent,KineticsWidget):
            QtGui.QGraphicsEllipseItem.__init__(self,x,y,w,h)
        elif isinstance(parent,ComptRect):
            QtGui.QGraphicsEllipseItem.__init__(self,x,y,w,h,parent)
            self.layoutWidgetpt = parent.pointerLayoutpt()
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable, True );        
        self.setFlag(QtGui.QGraphicsItem.ItemSendsScenePositionChanges, True );
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
    
    def pointerLayoutpt(self):
        return (self.layoutWidgetPt)
    def mouseDoubleClickEvent(self, event):
        self.ellemitter.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),element(self.mooseObj_))
    
    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.ellemitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),element(self.mooseObj_))
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.ellemitter.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),element(self.mooseObj_))
        return QtGui.QGraphicsItem.itemChange(self,change,value)
    
    def paint(self, painter, option, widget):
        if(self.mooseObj_.class_ == 'ZombieReac'):
            x = self.x
            y = self.y
            width = self.sceneBoundingRect().width()
            height = self.sceneBoundingRect().height()
            painter.setPen(QtGui.QPen(QtGui.QBrush(Qt.Qt.black),1))
            arrow = QtGui.QPolygonF([QtCore.QPointF(x,y+height/2),QtCore.QPointF(x+width,y+height/2)])
            painter.drawPolygon(arrow)
            arrow = QtGui.QPolygonF([QtCore.QPointF(x,y+height/2),QtCore.QPointF(x+(math.sin(90)*8),y+height/2+math.cos(90)*8)])
            painter.drawPolygon(arrow)
            arrow = QtGui.QPolygonF([QtCore.QPointF(x+width,y+height/2),QtCore.QPointF(x+width+(math.sin(225)*8),y+height/2+math.cos(225)*8)])
            painter.drawPolygon(arrow)
        else:
            QtGui.QGraphicsEllipseItem.paint(self,painter,option,widget)

class ComptRect(QtGui.QGraphicsRectItem):
    def __init__(self,parent,x,y,w,h,item):
        self.Rectemitter = QtCore.QObject()
        self.mooseObj_ = item[0].parent
        self.layoutWidgetPt = parent
        QtGui.QGraphicsRectItem.__init__(self,x,y,w,h)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        if config.QT_MINOR_VERSION >= 6:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 

    def pointerLayoutpt(self):
        return (self.layoutWidgetPt)
    
    def mouseDoubleClickEvent(self, event):
        self.Rectemitter.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.mooseObj_)
    
    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.Rectemitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mooseObj_)
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.Rectemitter.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.mooseObj_)
        return QtGui.QGraphicsItem.itemChange(self,change,value)


class GraphicalView(QtGui.QGraphicsView):
    def __init__(self,parent,border):
        QtGui.QGraphicsView.__init__(self,parent)
        self.setScene(parent)
        self.sceneContainerPt = parent
        self.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
        self.itemSelected = False
        self.customrubberBand=0
        self.rubberbandWidth = 0
        self.rubberbandHeight = 0
        self.moved = False
        self.showpopupmenu = False
        self.border = 6
        self.setRenderHints(QtGui.QPainter.Antialiasing)
    #def resizeevent()

    def mousePressEvent(self, event):
        if event.buttons() == QtCore.Qt.LeftButton:
            self.startingPos = event.pos()
            self.startScenepos = self.mapToScene(self.startingPos)
            self.deviceTransform = self.viewportTransform()
            if config.QT_MINOR_VERSION >= 6:
                # deviceTransform needs to be provided if the scene contains items that ignore transformations,which was introduced in 4.6
                sceneitems = self.sceneContainerPt.itemAt(self.startScenepos,self.deviceTransform)
            else:
                #for below  Qt4.6 there is no view transform for itemAt 
                #and if view is zoom out below 50%  and if textitem object is moved, zooming also happens.
                sceneitems = self.sceneContainerPt.itemAt(self.startScenepos)

            #checking if mouse press position is on any item (in my case textitem or rectcompartment) if none, 
            if ( sceneitems == None):
                QtGui.QGraphicsView.mousePressEvent(self, event)
                #Since qgraphicsrectitem is a item in qt, if I select inside the rectangle it would select the entire
                #rectangle and would not allow me to select the items inside the rectangle so breaking the code by not
                #calling parent class to inherit functionality rather writing custom code for rubberband effect here
            else:
                #print "s",sceneitems
                if( (isinstance(sceneitems, Textitem)) or (isinstance(sceneitems, Rectcplx)) or (isinstance(sceneitems, EllipseItem)) ):
                    #print "sceneitems",sceneitems
                    QtGui.QGraphicsView.mousePressEvent(self, event)
                    self.itemSelected = True

                elif(isinstance(sceneitems, ComptRect)):
                    for previousSelection in self.sceneContainerPt.selectedItems():
                        if previousSelection.isSelected() == True:
                            previousSelection.setSelected(0)
                    #Checking if its on the border or inside
                    xs = sceneitems.mapToScene(sceneitems.boundingRect().topLeft()).x()+self.border/2
                    ys = sceneitems.mapToScene(sceneitems.boundingRect().topLeft()).y()+self.border/2
                    xe = sceneitems.mapToScene(sceneitems.boundingRect().bottomRight()).x()-self.border/2
                    ye = sceneitems.mapToScene(sceneitems.boundingRect().bottomRight()).y()-self.border/2
                    xp = self.startScenepos.x()
                    yp = self.startScenepos.y()
                    
                    #if on border rubberband is not started, but called parent class for default implementation
                    if(((xp > xs-self.border/2) and (xp < xs+self.border/2) and (yp > ys-self.border/2) and (yp < ye+self.border/2) )or ((xp > xs+self.border/2) and (xp < xe-self.border/2) and (yp > ye-self.border/2) and (yp < ye+self.border/2) ) or ((xp > xs+self.border/2) and (xp < xe-self.border/2) and (yp > ys-self.border/2) and (yp < ys+self.border/2) ) or ((xp > xe-self.border/2) and (xp < xe+self.border/2) and (yp > ys-self.border/2) and (yp < ye+self.border/2) ) ):
                        QtGui.QGraphicsView.mousePressEvent(self, event)
                        self.itemSelected = True
                        if sceneitems.isSelected() == False:
                            sceneitems.setSelected(1)
                        #if its inside the qgraphicsrectitem then custom code for starting rubberband selection 
                    else:
                        self.customrubberBand = QtGui.QRubberBand(QtGui.QRubberBand.Rectangle,self)
                        self.customrubberBand.setGeometry(QtCore.QRect(self.startingPos,QtCore.QSize()))
                        self.customrubberBand.show()
    def mouseMoveEvent(self,event):
        QtGui.QGraphicsView.mouseMoveEvent(self, event)
        if( (self.customrubberBand) and (event.buttons() == QtCore.Qt.LeftButton)):
            self.endingPos = event.pos()
            self.endingScenepos = self.mapToScene(self.endingPos)
            self.rubberbandWidth = self.endingScenepos.x()-self.startScenepos.x()
            self.rubberbandHeight = self.endingScenepos.y()-self.startScenepos.y()
            self.customrubberBand.setGeometry(QtCore.QRect(self.startingPos, event.pos()).normalized())
            #unselecting any previosly selected item in scene
            for preSelectItem in self.sceneContainerPt.selectedItems():
                preSelectItem.setSelected(0)
            #since it custom rubberband I am checking if with in the selected area any textitem, if s then setselected to true
            for items in self.sceneContainerPt.items(self.startScenepos.x(),self.startScenepos.y(),self.rubberbandWidth,self.rubberbandHeight,Qt.Qt.IntersectsItemShape):
                if(isinstance(items,Textitem) or isinstance(items, Rectcplx) or isinstance(items, EllipseItem)):
                    if items.isSelected() == False:
                        items.setSelected(1)
                        
    def mouseReleaseEvent(self, event):
        self.setCursor(Qt.Qt.ArrowCursor)
        QtGui.QGraphicsView.mouseReleaseEvent(self, event)
        if(self.customrubberBand):
            self.customrubberBand.hide()
            self.customrubberBand = 0
        if event.button() == QtCore.Qt.LeftButton and self.itemSelected == False :
            self.endingPos = event.pos()
            self.endScenepos = self.mapToScene(self.endingPos)
            self.rubberbandWidth = (self.endScenepos.x()-self.startScenepos.x())
            self.rubberbandHeight = (self.endScenepos.y()-self.startScenepos.y())
            selecteditems = self.sceneContainerPt.selectedItems()
            if self.rubberbandWidth != 0 and self.rubberbandHeight != 0 and len(selecteditems) != 0 :
                self.showpopupmenu = True
        self.itemSelected = False
        if self.showpopupmenu:
            #Check if entire rect is selected then also it shd work
            popupmenu = QtGui.QMenu('PopupMenu', self)
            self.zoom = QtGui.QAction(self.tr('zoom'), self)
            self.connect(self.zoom, QtCore.SIGNAL('triggered()'), self.zoomItem)
            self.move = QtGui.QAction(self.tr('move'), self)
            self.connect(self.move, QtCore.SIGNAL('triggered()'), self.moveItem)
            #~ popupmenu.addAction(self.delete)
            popupmenu.addAction(self.zoom)
            popupmenu.addAction(self.move)
            popupmenu.exec_(event.globalPos())
        self.showpopupmenu = False
    
    def moveItem(self):
      self.setCursor(Qt.Qt.CrossCursor)

    def zoomItem(self):
        vTransform = self.viewportTransform()
        if( self.rubberbandWidth > 0  and self.rubberbandHeight >0):
            self.rubberbandlist = self.sceneContainerPt.items(self.startScenepos.x(),self.startScenepos.y(),self.rubberbandWidth,self.rubberbandHeight, Qt.Qt.IntersectsItemShape)
            for unselectitem in self.rubberbandlist:
                if unselectitem.isSelected() == True:
                    unselectitem.setSelected(0)
            for items in (qgraphicsitem for qgraphicsitem in self.rubberbandlist if isinstance(qgraphicsitem,Textitem) or isinstance(qgraphicsitem,Rectcplx) or isinstance(qgraphicsitem, EllipseItem)):
                self.fitInView(self.startScenepos.x(),self.startScenepos.y(),self.rubberbandWidth,self.rubberbandHeight,Qt.Qt.KeepAspectRatio)
                if((self.matrix().m11()>=1.0)and(self.matrix().m22() >=1.0)):
                    for item in ( Txtitem for Txtitem in self.sceneContainerPt.items() if isinstance(Txtitem,Textitem) ):
                        item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)
        else:
            self.rubberbandlist = self.sceneContainerPt.items(self.endScenepos.x(),self.endScenepos.y(),abs(self.rubberbandWidth),abs(self.rubberbandHeight), Qt.Qt.IntersectsItemShape)
            for unselectitem in self.rubberbandlist:
                if unselectitem.isSelected() == True:
                    unselectitem.setSelected(0)
            for items in (qgraphicsitem for qgraphicsitem in self.rubberbandlist if isinstance(qgraphicsitem,Textitem) or isinstance(qgraphicsitem,Rectcplx) or isinstance(qgraphicsitem, EllipseItem)):
                self.fitInView(self.endScenepos.x(),self.endScenepos.y(),abs(self.rubberbandWidth),abs(self.rubberbandHeight),Qt.Qt.KeepAspectRatio)
                if((self.matrix().m11()>=1.0)and(self.matrix().m22() >=1.0)):
                    for item in ( Txtitem for Txtitem in self.sceneContainerPt.items() if isinstance (Txtitem, Textitem)):
                        item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)
        self.rubberBandactive = False


class Widgetvisibility(Exception):pass    
class KineticsWidget(QtGui.QWidget):
    def __init__(self,size,modelPath,parent=None):
        QtGui.QWidget.__init__(self,parent)
        self.border = 10
        hLayout = QtGui.QGridLayout(self)
        #print "Ava moose",size
        self.setLayout(hLayout)
        self.sceneContainer = QtGui.QGraphicsScene(self)
        self.sceneContainer.setBackgroundBrush(QtGui.QColor(230,220,219,120))
        self.view = GraphicalView(self.sceneContainer,self.border)
        cmptMol = {}
        maxX,minX,maxY,minY = self.setupCompt_Coord(modelPath,cmptMol)
        #for k,v in cmptMol.items(): print k,'\n',v
        if maxX - minX != 0:
            xnewratio = (size.width()-10)/(maxX-minX)
        else:
            xnewratio = size.width()-10
        
        if maxY - minY != 0:
            ynewratio = (size.height()-10)/(maxY-minY)
        else:
            ynewratio = size.height()-10
        #print size,maxX,minX,maxY,minY,"xnw",xnewratio,"ynew",ynewratio
        self.srcdesConnection = {}
        zombieType = ['ZombieReac','ZombieEnz','ZombieMMenz','ZombieSumFunc']
        self.setupItem(modelPath,zombieType,self.srcdesConnection)
        #for m,n in self.srcdesConnection.items():print m,n
        #pickled the color map here and loading the file
        pkl_file = open(os.path.join(PATH_KKIT_COLORMAPS,'rainbow2.pkl'),'rb')
        self.picklecolorMap = pickle.load(pkl_file)        
        self.lineItem_dict = {}
        self.object2line = {}
        #This is check which version of kkit, b'cos anything below kkit8 didn't had xyz co-ordinates
        allZero = "True"
        for cmpt,itemlist in cmptMol.items():
            for  item in (item for item in itemlist if len(item) != 0):
                if item[1] != 0.0 or item[2] != 0.0:
                    allZero = False
                    break
        if allZero:
            msgBox = QtGui.QMessageBox()
            msgBox.setText("The Layout module works for kkit version 8 or higher.")
            msgBox.setStandardButtons(QtGui.QMessageBox.Ok)
            msgBox.exec_()
            raise Widgetvisibility()
        else:
            fnt = QtGui.QFont('Helvetica',8)
            self.qGraCompt = {}
            self.mooseId_GText = {}
            self.ellipse_width = 15
            self.ellipse_height = 15
            self.cplx_width = 8
            self.cplx_height = 8
            for cmpt,itemlist in cmptMol.items():
                self.createCompt(cmpt)
                comptRef = self.qGraCompt[cmpt]
                # for  item in (item for item  in itemlist if len(item) != 0): # THIS IS HORRIBLE!!!
                for item in itemlist:
                    if len(item) == 0:
                        continue
                    if item[0].class_ == 'ZombieEnz' or item[0].class_ == 'ZombieMMenz' or item[0].class_ == 'ZombieReac':
                        iteminfo = (element(item[0]).parent).path+'/info'
                        xpos = (item[1]-minX)*xnewratio
                        ypos = -(item[2]-minY)*ynewratio
                        if item[0].class_ == 'ZombieReac':
                            reItem = EllipseItem(xpos,ypos,self.ellipse_width,self.ellipse_height,comptRef,item)

                        else:
                            reItem = EllipseItem(xpos,ypos,self.ellipse_width,self.ellipse_height,comptRef,item)
                            textcolor = ''
                            bgcolor = Annotator(iteminfo).getField('color')
                            textcolor,bgcolor = self.colorCheck(textcolor,bgcolor,self.picklecolorMap)
                            reItem.setBrush(QtGui.QColor(bgcolor))
                        reItem.ellemitter.connect(reItem.ellemitter, QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.emitItemtoEditor)
                        reItem.ellemitter.connect(reItem.ellemitter,QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.emitItemtoEditor)
                        reItem.ellemitter.connect(reItem.ellemitter,QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.positionChange)
                        self.mooseId_GText[element(item[0]).getId()] = reItem

                    elif item[0].class_ == 'ZombiePool' or item[0].class_ == 'ZombieFuncPool' or item[0].class_ == 'ZombieBufPool':
                        xpos = (item[1]-minX)*xnewratio
                        ypos = -(item[2]-minY)*ynewratio
                        if item[0][0].parent.class_ != 'ZombieEnz':
                            pItem = Textitem(comptRef,item,xpos,ypos,self.picklecolorMap)
                            pItem.setFont(fnt)
                            pItem.textemitter.connect(pItem.textemitter, QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.emitItemtoEditor)
                            pItem.textemitter.connect(pItem.textemitter,QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.emitItemtoEditor)
                            pItem.textemitter.connect(pItem.textemitter,QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.positionChange)
                        else:
                            #cplx has made to sit under enz, for which xpos added with width/2 and height is added by ellipseitem height which is 15 for now
                            xpos = xpos+(self.cplx_width/2)
                            ypos = ypos+self.ellipse_width
                            pItem = Rectcplx(xpos,ypos,self.cplx_width,self.cplx_height,self.mooseId_GText[element(item[0]).parent.getId()],item[0])
                            textcolor = ''
                            pItem.Rectemitter1.connect(pItem.Rectemitter1,QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.positionChange)
                            pItem.Rectemitter1.connect(pItem.Rectemitter1,QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.emitItemtoEditor)
                            pItem.Rectemitter1.connect(pItem.Rectemitter1,QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.emitItemtoEditor)
                        self.mooseId_GText[element(item[0]).getId()] = pItem
        for k, v in self.qGraCompt.items():
            rectcompt = v.childrenBoundingRect()
            v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))
            v.setPen(QtGui.QPen(Qt.QColor(66,66,66,100),10,QtCore.Qt.SolidLine,QtCore.Qt.RoundCap,QtCore.Qt.RoundJoin ))
            v.Rectemitter.connect(v.Rectemitter,QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.positionChange)
            v.Rectemitter.connect(v.Rectemitter,QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.emitItemtoEditor)                
            v.Rectemitter.connect(v.Rectemitter,QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.emitItemtoEditor)
        for inn,out in self.srcdesConnection.items():
            if ((len(filter(lambda x:isinstance(x,list), out))) != 0):
                for items in (items for items in out[0] ):
                    src = ""
                    des = ""
                    src = self.mooseId_GText[inn]
                    des = self.mooseId_GText[element(items[0]).getId()]
                    self.lineCord(src,des,items[1])
                for items in (items for items in out[1] ):
                    src = ""
                    des = ""
                    src = self.mooseId_GText[inn]
                    des = self.mooseId_GText[element(items[0]).getId()]
                    self.lineCord(src,des,items[1])
            else:
                for items in (items for items in out ):
                    src = ""
                    des = ""
                    src = self.mooseId_GText[element(inn).getId()]
                    des = self.mooseId_GText[element(items[0]).getId()]
                    self.lineCord(src,des,items[1])        
        #self.view.fitInView(self.sceneContainer.sceneRect().x()-10,self.sceneContainer.sceneRect().y()-10,self.sceneContainer.sceneRect().width()+20,self.sceneContainer.sceneRect().height()+20,Qt.Qt.KeepAspectRatio)
        self.view.fitInView(self.sceneContainer.itemsBoundingRect().x()-10,self.sceneContainer.itemsBoundingRect().y()-10,self.sceneContainer.itemsBoundingRect().width()+20,self.sceneContainer.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
        hLayout.addWidget(self.view)
        
    
    def lineCord(self,src,des,endtype):
        source = element(next((k for k,v in self.mooseId_GText.items() if v == src), None))
        desc = element(next((k for k,v in self.mooseId_GText.items() if v == des), None))
        line = 0
        if( (src == "") and (des == "") ):
            print "Source or destination is missing or incorrect"
        else:
            srcdes_list= [src,des,endtype]
            arrow = self.calArrow(src,des,endtype)
            if(source.class_ == "ZombieReac"):
                qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.green, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
                line = 1
            elif( (source.class_ == "ZombieEnz") or (source.class_ == "ZombieMMenz")):
                
                if ( (endtype == 's') or (endtype == 'p')):
                    qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.red, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
                    line = 1
                elif(endtype != 'cplx'):
                    p = element(next((k for k,v in self.mooseId_GText.items() if v == src), None)) 
                    parentinfo = p.path+'/info'
                    textColor = Annotator(parentinfo).getField('textColor')
                    if(isinstance(textColor,(list,tuple))):
                        r,g,b = textColor[0],textColor[1],textColor[2]
                        color = QtGui.QColor(r,g,b)
                    elif ((not isinstance(textColor,(list,tuple)))):
                        if textColor.isdigit():
                            tc = int(textColor)
                            tc = (tc * 2 )
                            r,g,b = self.picklecolorMap[tc]
                            color = QtGui.QColor(r,g,b)
                        else: 
                            color = QtGui.QColor(200,200,200)
                    qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(color ,1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
                    line = 1
                elif(endtype == 'cplx'):
                    pass
            elif( (source.class_ == "ZombiePool") or (source.class_ == "ZombieFuncPool") or (source.class_ == "ZombieBuffPool")):
                qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.blue, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
                line =1
            if line == 1:            
                self.lineItem_dict[qgLineitem] = srcdes_list
                if src in self.object2line:
                    self.object2line[ src ].append( ( qgLineitem, des) )
                else:
                    self.object2line[ src ] = []
                    self.object2line[ src ].append( ( qgLineitem, des) )
                if des in self.object2line:
                    self.object2line[ des ].append( ( qgLineitem, src ) )
                else:
                    self.object2line[ des ] = []
                    self.object2line[ des ].append( ( qgLineitem, src) )
    
    def updateItemSlot(self, mooseObject):
        #In this case if the name is updated from the keyboard both in mooseobj and gui gets updation
        changedItem = ''
        for changedItem in (item for item in self.sceneContainer.items() if isinstance(item, Textitem) and mooseObject.getId() == item.mooseObj_.getId()):
            break
        
        if isinstance(changedItem,Textitem):
            changedItem.updateSlot()
            #once the text is edited in editor, width gets resized for the positionChange signal shd be emitted"
            self.positionChange(changedItem.mooseObj_)
            
    def updatearrow(self,qGTextitem):
        #if there is no arrow to update then return
        if qGTextitem not in self.object2line:
            return
        listItem = self.object2line[qGTextitem]
        for ql, va in listItem:
            srcdes = self.lineItem_dict[ql]
            forloop = 0
            if(isinstance(srcdes[0],EllipseItem)):
                pItem = (next((k for k,v in self.mooseId_GText.items() if v == srcdes[0]), None))
                mooseObj = (next((k for k,v in self.mooseId_GText.items() if v == srcdes[1]), None))
                for l1 in self.srcdesConnection[pItem]:
                    for k in l1:
                        if ((k[0]) == mooseObj):   
                            endtype = k[1]
                        else:
                            if isinstance(qGTextitem,EllipseItem):
                                gItem = self.mooseId_GText[k[0]]
                                self.updatearrow(gItem)
               
            elif(isinstance(srcdes[1],EllipseItem)):
                pItem = (next((k for k,v in self.mooseId_GText.items() if v == srcdes[1]), None))
                mooseObject = (next((k for k,v in self.mooseId_GText.items() if v == srcdes[0]), None))
                for l1 in self.srcdesConnection[pItem]:
                    for k in l1:
                        if (k[0]) == mooseObj:
                            endtype = k[1]
            else:
                pItem  =  (next((k for k,v in self.mooseId_GText.items() if v == srcdes[0]), None))
                pItem1 =  (next((k for k,v in self.mooseId_GText.items() if v == srcdes[1]), None))
                if(pItem.class_ == 'ZombieFuncPool' or pItem1.class_ == 'ZombieFuncPool'):
                    endtype = 'st'

            arrow = self.calArrow(srcdes[0],srcdes[1],endtype)
            ql.setPolygon(arrow)
      
    def calArrow(self,src,des,endtype):
        sX = src.sceneBoundingRect().x()
        sY = src.sceneBoundingRect().y()
        sw = src.sceneBoundingRect().right() -src.sceneBoundingRect().left()
        sh = src.sceneBoundingRect().bottom() -src.sceneBoundingRect().top()
        
        dX = des.sceneBoundingRect().x()
        dY = des.sceneBoundingRect().y()
        dw = des.sceneBoundingRect().right() -des.sceneBoundingRect().left()
        dh = des.sceneBoundingRect().bottom() -des.sceneBoundingRect().top()

        #Here there is external boundary created for each textitem 
        #1. for checking if there is overLap
        #2. The start line and arrow head ends to this outer boundary
        srcRect = QtCore.QRectF(sX,sY,sw,sh)
        desRect = QtCore.QRectF(dX,dY,dw,dh)
        #To see if the 2 boundary rect of text item intersects
        t = srcRect.intersects(desRect)
        arrow = QtGui.QPolygonF()
        if not t:
            centerPoint = QtCore.QLineF(src.sceneBoundingRect().center().x(),src.sceneBoundingRect().center().y(),des.sceneBoundingRect().center().x(),des.sceneBoundingRect().center().y())
            lineSrcpoint = QtCore.QPointF(0,0)
            srcAngle = self.calPoAng(sX,sY,sw,sh,centerPoint,lineSrcpoint)
            lineDespoint = QtCore.QPointF(0,0)
            self.calPoAng(dX,dY,dw,dh,centerPoint,lineDespoint)
            # src and des are connected with line co-ordinates
            #Arrow head is drawned if the distance between src and des line is >8 just for clean appeareance
            if(endtype == 'p'):
                if(abs(lineSrcpoint.x()-lineDespoint.x()) > 8 or abs(lineSrcpoint.y()-lineDespoint.y())>8):
                    #Arrow head for Source is calculated
                    srcAngle = self.calPoAng(dX,dY,dw,dh,centerPoint,lineDespoint)
                    degree = -60
                    srcXArr1,srcYArr1= self.arrowHead(srcAngle,degree,lineDespoint)
                    degree = -120
                    srcXArr2,srcYArr2 = self.arrowHead(srcAngle,degree,lineDespoint)
                    arrow.append(QtCore.QPointF(lineSrcpoint.x(),lineSrcpoint.y()))
                    arrow.append(QtCore.QPointF(lineDespoint.x(),lineDespoint.y()))
                    
                    arrow.append(QtCore.QPointF(srcXArr1,srcYArr1))
                    arrow.append(QtCore.QPointF(lineDespoint.x(),lineDespoint.y()))
                    arrow.append(QtCore.QPointF(srcXArr2,srcYArr2))                    
                    arrow.append(QtCore.QPointF(lineDespoint.x(),lineDespoint.y()))
            else:
                arrow.append(QtCore.QPointF(lineSrcpoint.x(),lineSrcpoint.y()))
                arrow.append(QtCore.QPointF(lineDespoint.x(),lineDespoint.y()))
                                    
            return (arrow)
        elif t:
            # This is created for getting a emptyline for reference b'cos 
            # lineCord function add qgraphicsline to screen and also add's a ref for src and des
            arrow.append(QtCore.QPointF(0,0))
            arrow.append(QtCore.QPointF(0,0))
            return (arrow)

    #checking which side of rectangle intersect with other
    def calPoAng(self,X,Y,w,h,centerLine,linePoint):
            #Here the 1. a. intersect point between center and 4 sides of src and 
            #            b. intersect point between center and 4 sides of des and to draw a line connecting for src & des
            #         2. angle for src for the arrow head calculation is returned
            #  Added & substracted 5 for x and y making a outerboundary for the item for drawing
            lItemSP = QtCore.QLineF(X-5,Y-5,X+w+5,Y-5)
            boundintersect= lItemSP.intersect(centerLine,linePoint)
            if (boundintersect == 1):
                return centerLine.angle()
            else:
                lItemSP = QtCore.QLineF(X+w+5,Y-5,X+w+5,Y+h+5)
                boundintersect= lItemSP.intersect(centerLine,linePoint)
                if (boundintersect == 1):
                    return centerLine.angle()
                else:
                    lItemSP = QtCore.QLineF(X+w+5,Y+h+5,X-5,Y+h+5)
                    boundintersect= lItemSP.intersect(centerLine,linePoint)
                    if (boundintersect == 1):
                        return centerLine.angle()
                    else:
                        lItemSP = QtCore.QLineF(X-5,Y+h+5,X-5,Y-5)
                        boundintersect= lItemSP.intersect(centerLine,linePoint)
                        if (boundintersect == 1):
                            return centerLine.angle()
                        else:
                            linePoint = QtCore.QPointF(0,0)
                            return 0

    #arrow head is calculated
    def arrowHead(self,srcAngle,degree,lineSpoint):
        r = 8
        delta = math.radians(srcAngle) + math.radians(degree)
        width = math.sin(delta)*r
        height = math.cos(delta)*r
        srcXArr = lineSpoint.x() + width
        srcYArr = lineSpoint.y() + height
        return srcXArr,srcYArr

    def emitItemtoEditor(self,mooseObject):
        self.emit(QtCore.SIGNAL("itemDoubleClicked(PyQt_PyObject)"),mooseObject)

    def positionChange(self,mooseObject):
        #If the item position changes, the corresponding arrow's are claculated
       if ( mooseObject.class_ == 'ZombiePool' or mooseObject.class_ == 'ZombieFuncPool' or mooseObject.class_ == 'ZombieSumFunc' or mooseObject.class_ == 'ZombieBufPool'):
            pool = self.mooseId_GText[mooseObject.getId()]
            self.updatearrow(pool)
            for k, v in self.qGraCompt.items():
                rectcompt = v.childrenBoundingRect()
                v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))
       else:
            if((mooseObject.class_ == 'ZombieMMenz') or (mooseObject.class_ == 'ZombieEnz') or (mooseObject.class_ == 'ZombieReac') ):
                refenz = self.mooseId_GText[mooseObject.getId()]
                self.updatearrow(refenz)
                for k, v in self.qGraCompt.items():
                    rectcompt = v.childrenBoundingRect()
                    v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))
    
            else:

                if mooseObject.class_ == "CubeMesh":
                    for k, v in self.qGraCompt.items():
                        mesh = mooseObject.path+'/mesh'
                        if k.path == mesh:
                            for rectChilditem in v.childItems():
                                self.updatearrow(rectChilditem)

    def colorCheck(self,textColor,bgcolor,pklcolor):
        if(textColor == ''): textColor = 'green'
        if(bgcolor == ''): bgcolor = 'blue'
        if(textColor == bgcolor): textColor = self.randomColor()
        hexchars = "0123456789ABCDEF"
        if(isinstance(textColor,(list,tuple))):
            r,g,b = textColor[0],textColor[1],textColor[2]
            textColor = "#"+ hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16]
        elif ((not isinstance(textColor,(list,tuple)))):
            if textColor.isdigit():
                tc = int(textColor)
                tc = (tc * 2 )
                r,g,b = pklcolor[tc]
                textColor = "#"+ hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16]
        if ((not isinstance(bgcolor,(list,tuple)))):
            if bgcolor.isdigit():
                tc = int(bgcolor)
                tc = (tc * 2 )
                r,g,b = pklcolor[tc]
                bgcolor = "#"+hexchars[r/16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16]
        return(textColor,bgcolor)
   
    def randomColor(self):
        red = int(random.uniform(0, 255))
        green = int(random.uniform(0, 255))
        blue = int(random.uniform(0, 255))
        return (red,green,blue)
     
    def createCompt(self,key):
        self.new_Compt = ComptRect(self,0,0,0,0,key)
        self.qGraCompt[key] = self.new_Compt
        self.new_Compt.setRect(10,10,10,10)
        self.sceneContainer.addItem(self.new_Compt)

    def setupItem(self,modlePath,searObject,cntDict):
        for zombieObj in searObject:
            path = modlePath+'/##[TYPE='+zombieObj+']'
            if zombieObj != 'ZombieSumFunc':
                for items in wildcardFind(path):
                    sublist = []
                    prdlist = []
                    for sub in items[0].getNeighbors('sub'): 
                        sublist.append((sub,'s'))
                    for prd in items[0].getNeighbors('prd'):
                        prdlist.append((prd,'p'))
                    if (zombieObj == 'ZombieEnz') :
                        for enzpar in items[0].getNeighbors('toEnz'):
                            sublist.append((enzpar,'t'))
                        for cplx in items[0].getNeighbors('cplxDest'):
                            prdlist.append((cplx,'cplx'))
                    if (zombieObj == 'ZombieMMenz'):
                        for enzpar in items[0].getNeighbors('enzDest'):
                            sublist.append((enzpar,'t'))
                    cntDict[items] = sublist,prdlist
            else:
                #ZombieSumFunc adding inputs
                for items in wildcardFind(path):
                    inputlist = []
                    outputlist = []
                    funplist = []
                    nfunplist = []
                    for inpt in items[0].getNeighbors('input'):
                        inputlist.append((inpt,'st'))
                    for zfun in items[0].getNeighbors('output'): funplist.append(zfun)
                    for i in funplist: nfunplist.append(element(i).getId())
                    nfunplist = list(set(nfunplist))
                    if(len(nfunplist) > 1): print "SumFunPool has multiple Funpool"
                    else:
                        for el in funplist:
                            if(element(el).getId() == nfunplist[0]):
                                cntDict[element(el)] = inputlist
                                break

    def setupCompt_Coord (self,filePath,mobject_Cord):
        cPath = filePath+'/##[TYPE=MeshEntry]'
        xratio = 0
        yratio = 0
        x = []
        y = []
            
        for meshEnt in wildcardFind(cPath):
            molrecList = []
            pkl_file = open(os.path.join(PATH_KKIT_COLORMAPS,'rainbow2.pkl'),'rb')
            picklecolorMap = pickle.load(pkl_file)
            for reitem in Neutral(meshEnt).getNeighbors('remeshReacs'):
                reiteminfo = reitem.path+'/info'
                xxr = float(element(reiteminfo).getField('x'))
                yyr = float(element(reiteminfo).getField('y'))                
                textcolor = Annotator(reiteminfo).getField('textColor')
                bgcolor  =  Annotator(reiteminfo).getField('color')
                textcolor,bgcolor = self.colorCheck(textcolor,bgcolor,picklecolorMap)
                molrecList.append((reitem,xxr,yyr,textcolor,bgcolor))
                x.append(xxr)
                y.append(yyr)
            for mitem in Neutral(meshEnt).getNeighbors('remesh'):
                if ( (mitem[0].class_ != 'GslIntegrator')):
                    if ((mitem[0].parent).class_ == 'ZombieEnz'):
                        miteminfo = (mitem[0].parent).path+'/info'
                        xx = float(element(miteminfo).getField('x'))
                        yy = float(element(miteminfo).getField('y'))
                        textcolor = Annotator(miteminfo).getField('textColor')
                        bgcolor = Annotator(miteminfo).getField('color')
                        textcolor,bgcolor = self.colorCheck(textcolor,bgcolor,picklecolorMap)
                        molrecList.append((mitem, xx,yy,textcolor,bgcolor))
                        x.append(xx)
                        y.append(yy)
                    else:
                        miteminfo = mitem.path+'/info'
                        xx1 = float(element(miteminfo).getField('x'))
                        yy1 = float(element(miteminfo).getField('y'))
                        textcolor = Annotator(miteminfo).getField('textColor')
                        bgcolor = Annotator(miteminfo).getField('color')
                        textcolor,bgcolor = self.colorCheck(textcolor,bgcolor,picklecolorMap)
                        molrecList.append((mitem,xx1,yy1,textcolor,bgcolor))
                        x.append(xx1)
                        y.append(yy1)
                    #print "$",x[-1],y[-1]
            mobject_Cord[meshEnt] = molrecList
        xratio = ((max(x))-(min(x)))
        yratio = ((max(y))-(min(y)))
        return(max(x),min(x),max(y),min(y))
    
    def keyPressEvent(self,event):
        key = event.key()
        if key == QtCore.Qt.Key_A:
            self.view.fitInView(self.sceneContainer.itemsBoundingRect().x()-10,self.sceneContainer.itemsBoundingRect().y()-10,self.sceneContainer.itemsBoundingRect().width()+20,self.sceneContainer.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
            #self.view.fitInView(self.sceneContainer.sceneRect().x()-10,self.sceneContainer.sceneRect().y()-10,self.sceneContainer.sceneRect().width()+20,self.sceneContainer.sceneRect().height()+20,Qt.Qt.IgnoreAspectRatio)
        elif (key == 46 or key == 62):
            self.view.scale(1.1,1.1)
        elif (key == 44 or key == 60):
            self.view.scale(1/1.1,1/1.1)

        
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    size = QtCore.QSize(992, 704)
    modelPath = '77'
    modelPath = 'Kholodenko'
    modelPath = 'motors8'
    #modelPath = 'enz_classical_explicit'
    #modelPath = 'reaction1'
    #modelPath = 'test_enzyme'
    #modelPath = 'OSC_Cspace'
    #modelPath = 'osc1'
    #modelPath = 'traff_nn_diff_TRI'
    #modelPath = 'traff_nn_diff_BIS'
    #modelPath = 'EGFR_MAPK_58'
    #modelPath = 'acc68'
  
    try:
        filepath = '../Demos/Genesis_files/'+modelPath+'.g'
        f = open(filepath, "r")
        loadModel(filepath,'/'+modelPath)
        dt = KineticsWidget(size,'/'+modelPath)
        dt.show()
  
    except  IOError, what:
      (errno, strerror) = what
      print "Error number", errno, "(%s)" % strerror
      sys.exit(0)    
    sys.exit(app.exec_())
