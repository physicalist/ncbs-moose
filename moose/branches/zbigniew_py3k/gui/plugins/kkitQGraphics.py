from PyQt4 import QtGui, QtCore, Qt
import config
from moose import *

class KineticsDisplayItem(QtGui.QGraphicsWidget):
    """Base class for display elemenets in kinetics layout"""
    def __init__(self, mooseObject, parent=None):
        QtGui.QGraphicsObject.__init__(self, parent)
        self.mobj = mooseObject
        self.gobj = None
        self.pressed = False
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable,True)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        if QtCore.QT_VERSION >= 0x040600:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges,1)

    def setDisplayProperties(self, dinfo):
        self.setGeometry(dinfo.x, dinfo.y)
      
    def paint(self, painter=None, option=None, widget = None):
        #If item is selected
        if self.hasFocus() or self.isSelected():
            painter.setPen(QtGui.QPen(QtGui.QPen(QtCore.Qt.black, 1.8,Qt.Qt.DashLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin)))
            painter.drawRect(self.boundingRect())
            

    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),element(self.mobj))
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),element(self.mobj))
        return QtGui.QGraphicsItem.itemChange(self,change,value)


class PoolItem(KineticsDisplayItem):
    """Class for displaying pools. Uses a QGraphicsSimpleTextItem to
    display the name."""    
    #fontMetrics = None
    defaultFontsize = 12
    font =QtGui.QFont("Helvetica")
    font.setPointSize(defaultFontsize)
    fontMetrics = QtGui.QFontMetrics(font)
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        self.bg = QtGui.QGraphicsRectItem(self)
        self.gobj = QtGui.QGraphicsSimpleTextItem(self.mobj.name, self.bg)        
        self.gobj.setFont(PoolItem.font)
        self.bgColor = QtGui.QGraphicsEllipseItem(self)
        self.bgColor.setFlag(QtGui.QGraphicsItem.ItemStacksBehindParent,True)
        self.bgColor.setRect(((self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width('  '))/2)-5,self.gobj.boundingRect().height()/2-5,10,10)
        if not PoolItem.fontMetrics:
            PoolItem.fontMetrics = QtGui.QFontMetrics(self.gobj.font())
        self.bg.setRect(0, 
                        0, 
                        self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width('  '), 
                        self.gobj.boundingRect().height())
        self.bg.setPen(Qt.QColor(0,0,0,0))
        self.gobj.setPos(PoolItem.fontMetrics.width(' '), 0)
    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x, y,self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width('  '), 
                        self.gobj.boundingRect().height())
        
        #self.gobj.setPen(QtGui.QPen(QtGui.QBrush(textcolor)))
        #self.gobj.setBrush(QtGui.QBrush(textcolor))

	if self.mobj.className != "StimulusTable":
            ## Changing back ground ellipse color and not setting rectItem
            self.bgColor.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor.red(),bgcolor.green(),bgcolor.blue(),255)))
            #Setting alpha value to be between 0-255
            #self.bg.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor.red(),bgcolor.green(),bgcolor.blue(),128)))
            #self.bg.setBrush(QtGui.QBrush(bgcolor))
            
    def refresh(self,scale):
        fontsize = PoolItem.defaultFontsize*scale
        font =QtGui.QFont("Helvetica")
        font.setPointSize(fontsize)
        self.gobj.setFont(font)

    def boundingRect(self):
        ''' reimplimenting boundingRect for redrawning '''
        return QtCore.QRectF(0,0,self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '),self.gobj.boundingRect().height())

    def updateSlot(self):
        self.gobj.setText(self.mobj[0].name)
        self.bg.setRect(0, 0, self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height())
    
    def updateColor(self,bgcolor):
        #self.bg.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor)))
        pass

    def updateRect(self,ratio):
        width = self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  ')
        height = self.gobj.boundingRect().height()
        adjustw = width*ratio
        adjusth = height*ratio
        self.bgColor.setRect(width/2-abs(adjustw/2),height/2-abs(adjusth/2),adjustw, adjusth)
        #self.bg.setRect(0,0,self.gobj.boundingRect().width()*ratio+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height()*ratio)

    def returnColor(self):
        return (self.bg.brush().color())

class TableItem(KineticsDisplayItem):
    defaultWidth = 30
    defaultHeight = 30
    defaultPenWidth = 2

    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        points = [QtCore.QPointF(0,TableItem.defaultWidth/2),
                  QtCore.QPointF(TableItem.defaultHeight/2-2,0),
                  QtCore.QPointF(TableItem.defaultWidth/2+2,0),
                  QtCore.QPointF(TableItem.defaultWidth,TableItem.defaultHeight/2),
                  ]

        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        path.moveTo(0,0)
        path.lineTo(TableItem.defaultWidth,0)
        path.moveTo(-(TableItem.defaultWidth/3),TableItem.defaultHeight/4)
        path.lineTo((TableItem.defaultWidth+10),TableItem.defaultHeight/4)

        self.gobj = QtGui.QGraphicsPathItem(path, self)
        self.gobj.setPen(QtGui.QPen(QtCore.Qt.black, 2,Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
    
    def refresh( self,scale):
        defaultWidth = TableItem.defaultWidth*scale
        defaultHeight = TableItem.defaultHeight*scale
        points = [QtCore.QPointF(0,defaultWidth/2),
                  QtCore.QPointF(defaultHeight/2-2,0),
                  QtCore.QPointF(defaultWidth/2+2,0),
                  QtCore.QPointF(defaultWidth,defaultHeight/2)
                  ]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        path.moveTo(0,0)
        path.lineTo(defaultWidth,0)
        path.moveTo(-(defaultWidth/3),defaultHeight/4)
        path.lineTo((defaultWidth+10),defaultHeight/4)
        self.gobj.setPath(path)
        TablePen = self.gobj.pen()
        defaultpenwidth = TableItem.defaultPenWidth
        tableWidth =  TableItem.defaultPenWidth*scale
        TablePen.setWidth(tableWidth)
        self.gobj.setPen(TablePen)

    def setDisplayProperties(self, x,y,textcolor,bgcolor):
        #TODO check the table bounding reactangle b'cos selection looks ugly
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())
    
class ReacItem(KineticsDisplayItem):
    defaultWidth = 30
    defaultHeight = 30
    defaultPenWidth = 2

    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        points = [QtCore.QPointF(ReacItem.defaultWidth/4, 0),
                  QtCore.QPointF(0, ReacItem.defaultHeight/4),
                  QtCore.QPointF(ReacItem.defaultWidth, ReacItem.defaultHeight/4),
                  QtCore.QPointF(3*ReacItem.defaultWidth/4, ReacItem.defaultHeight/2)]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        self.gobj = QtGui.QGraphicsPathItem(path, self)
        self.gobj.setPen(QtGui.QPen(QtCore.Qt.black, 2,Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
    
    def refresh( self,scale):
        defaultWidth = ReacItem.defaultWidth*scale
        defaultHeight = ReacItem.defaultHeight*scale
        points = [QtCore.QPointF(defaultWidth/4, 0),
                          QtCore.QPointF(0,defaultHeight/4),
                          QtCore.QPointF(defaultWidth, defaultHeight/4),
                          QtCore.QPointF(3*defaultWidth/4,defaultHeight/2)]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
                
        self.gobj.setPath(path)
        ReacPen = self.gobj.pen()
        defaultpenwidth = ReacItem.defaultPenWidth
        reacWidth =  ReacItem.defaultPenWidth*scale
        ReacPen.setWidth(reacWidth)
        self.gobj.setPen(ReacPen)
    def setDisplayProperties(self, x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())

class EnzItem(KineticsDisplayItem):
    defaultWidth = 20
    defaultHeight = 10    
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        self.gobj = QtGui.QGraphicsEllipseItem(0, 0, 
                                            EnzItem.defaultWidth, 
                                            EnzItem.defaultHeight, self)
        
    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())

        self.gobj.setBrush(QtGui.QBrush(textcolor))

    def refresh(self,scale):
        defaultWidth = EnzItem.defaultWidth*scale
        defaultHeight = EnzItem.defaultHeight*scale
        self.gobj.setRect(0,0,defaultWidth,defaultHeight)

class MMEnzItem(EnzItem):
    def __init__(self,*args, **kwargs):
        EnzItem.__init__(self,*args, **kwargs)

class CplxItem(KineticsDisplayItem):
    defaultWidth = 10
    defaultHeight = 10    
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        self.gobj = QtGui.QGraphicsRectItem(0,0, CplxItem.defaultWidth, CplxItem.defaultHeight, self)

    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(self.gobj.boundingRect().width()/2,self.gobj.boundingRect().height(), 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())

        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable,False)

    def refresh(self,scale):
        defaultWidth = CplxItem.defaultWidth*scale
        defaultHeight = CplxItem.defaultHeight*scale
	
        self.gobj.setRect(0,0,defaultWidth,defaultHeight)

class ComptItem(QtGui.QGraphicsRectItem):
    def __init__(self,parent,x,y,w,h,item):
        self.cmptEmitter = QtCore.QObject()
        iParent = item
        if hasattr(iParent, "__iter__"):
            self.mooseObj_ = iParent[0]
        else:
            self.mooseObj_ = iParent

        self.layoutWidgetPt = parent
        QtGui.QGraphicsRectItem.__init__(self,x,y,w,h)

        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable, True );
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        if config.QT_MINOR_VERSION >= 6:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
    
    def pointerLayoutpt(self):
        return (self.layoutWidgetPt)

    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.cmptEmitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mooseObj_)
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
            self.cmptEmitter.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.mooseObj_)
        return QtGui.QGraphicsItem.itemChange(self,change,value)

import sys
if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    a = moose.Pool('pool')
    b = moose.Reac('reac')
    c = moose.Enz('enzyme')
    gview = QtGui.QGraphicsView()
    scene = QtGui.QGraphicsScene(gview)
    #item = PoolItem(a)
    #dinfo = (5, 5, QtGui.QColor('red'), QtGui.QColor('blue'))
    #item.setDisplayProperties(dinfo)
    reacItem = ReacItem(b)
    reacItem.setDisplayProperties(50, 5, QtGui.QColor('yellow'), QtGui.QColor('green'))
    enzItem = EnzItem(c)
    enzItem.setDisplayProperties(100, 10, QtGui.QColor('blue'), QtGui.QColor('yellow'))
    #scene.addItem(item)
    scene.addItem(reacItem)
    scene.addItem(enzItem)
    gview.setScene(scene)
    print 'Position', reacItem.pos()
    gview.show()
    sys.exit(app.exec_())
