# LayoutWidget for Loading Genesis files
from moose import *
from PyQt4 import QtGui,QtCore,Qt
import sys
import math
import config
import objectedit

class Textitem(QtGui.QGraphicsTextItem): 
	positionChange = QtCore.pyqtSignal(QtGui.QGraphicsItem)
	def __init__(self,parent,path):
		self.mooseObj_ = path
		if isinstance (parent, LayoutWidget):
			QtGui.QGraphicsTextItem.__init__(self,self.mooseObj_.name)
			self.layoutWidgetpt = parent
			
		elif isinstance (parent,Rect_Compt):
			self.layoutWidgetpt = parent.pointerLayoutpt()
			QtGui.QGraphicsTextItem.__init__(self,parent)
			
		self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
		self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
		#self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
		if config.QT_MINOR_VERSION >= 6:
		 	self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 

	def itemChange(self, change, value):
		if change == QtGui.QGraphicsItem.ItemPositionChange:
			self.positionChange.emit(self)
       		return QtGui.QGraphicsItem.itemChange(self, change, value)
	
	def mouseDoubleClickEvent(self, event):
		objectedit.ObjectFieldsModel(self.mooseObj_)
		self.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.mooseObj_)
	
	def updateSlot(self):
		if(self.mooseObj_.className == 'Enzyme'):
			textColor = "<html><body bgcolor='black'>"+self.mooseObj_.name+"</body></html>"
			self.setHtml(textColor)
		else:
			textColor = self.mooseObj_.getField('xtree_fg_req')
			self.layoutWidgetpt.colorCheck(self,self.mooseObj_,textColor,"background")
		
class Rect_Compt(QtGui.QGraphicsRectItem):
	def __init__(self,layoutwidget,x,y,w,h,path):
		self.mooseObj_ = path
		self.layoutWidgetpt = layoutwidget
		self.Rectemitter = QtCore.QObject()
		QtGui.QGraphicsRectItem.__init__(self,x,y,w,h)
		self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
		self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
		
		#~ self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
		if config.QT_MINOR_VERSION >= 6:
		 	self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
		 	
	def mouseDoubleClickEvent(self, event):
		self.Rectemitter.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),self.mooseObj_)
	
	def itemChange(self, change, value):
		if change == QtGui.QGraphicsItem.ItemPositionChange:
			self.Rectemitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mooseObj_)
       		return QtGui.QGraphicsItem.itemChange(self, change, value)

	def pointerLayoutpt(self):
		return (self.layoutWidgetpt)
		
class Graphicalview(QtGui.QGraphicsView):
	def __init__(self,scenecontainer,border):
		self.sceneContainerPt = scenecontainer
		self.border = border
		QtGui.QGraphicsView.__init__(self,self.sceneContainerPt)
		self.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
		self.setScene(self.sceneContainerPt)
		self.itemSelected = False
		self.customrubberBand=0
		self.rubberbandWidth = 0
		self.rubberbandHeight = 0
		self.moved = False		
		self.showpopupmenu = False
		
		
	def mousePressEvent(self, event):
		if event.buttons() == QtCore.Qt.LeftButton:
			self.startingPos = event.pos()
			self.startScenepos = self.mapToScene(self.startingPos)
			
			self.deviceTransform = self.viewportTransform()

			if config.QT_MINOR_VERSION >= 6:
				''' deviceTransform needs to be provided if the scene contains items that ignore transformations,
					which was introduced in 4.6
				'''
				sceneitems = self.sceneContainerPt.itemAt(self.startScenepos,self.deviceTransform)
			else:
				''' for below  Qt4.6 there is no view transform for itemAt 
				     and if view is zoom out below 50%  and if textitem object is moved, 
				     zooming also happens.
				'''
				sceneitems = self.sceneContainerPt.itemAt(self.startScenepos)
					
			#checking if mouse press position is on any item (in my case textitem or rectcompartment) if none, 
			if ( sceneitems == None):
				QtGui.QGraphicsView.mousePressEvent(self, event)
				
				
			#Since qgraphicsrectitem is a item in qt, if I select inside the rectangle it would select the entire rectangle
			# and would not allow me to select the items inside the rectangle so breaking the code by not calling parent class to inherit functionality
			#rather writing custom code for rubberband effect here
			elif( sceneitems != None):
				if(isinstance(sceneitems, Textitem)):
					QtGui.QGraphicsView.mousePressEvent(self, event)
					self.itemSelected = True
				
				elif(isinstance(sceneitems, Rect_Compt)):
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
					if(((xp > xs-self.border/2) and (xp < xs+self.border/2) and (yp > ys-self.border/2) and (yp < ye+self.border/2) )or 
					   ((xp > xs+self.border/2) and (xp < xe-self.border/2) and (yp > ye-self.border/2) and (yp < ye+self.border/2) ) or 
					   ((xp > xs+self.border/2) and (xp < xe-self.border/2) and (yp > ys-self.border/2) and (yp < ys+self.border/2) ) or
					   ((xp > xe-self.border/2) and (xp < xe+self.border/2) and (yp > ys-self.border/2) and (yp < ye+self.border/2) ) ):
						
						QtGui.QGraphicsView.mousePressEvent(self, event)
						self.itemSelected = True
					   	if sceneitems.isSelected() == False:
					   		sceneitems.setSelected(1)
					   		
					#if its inside the qgraphicsrectitem then custom code for starting rubberband selection	 		
					else:
						self.customrubberBand = QtGui.QRubberBand(QtGui.QRubberBand.Rectangle,self)
						self.customrubberBand.setGeometry(QtCore.QRect(self.startingPos,QtCore.QSize()))
						self.customrubberBand.show()
						
				#~ else:
					#~ print "Report this functionality may not be implimentated"
					
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
					if(isinstance(items,Textitem)):
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
			#~ self.delete = QtGui.QAction(self.tr('delete'), self)
			#~ self.connect(self.delete, QtCore.SIGNAL('triggered()'), self.deleteItem)
			
			self.zoom = QtGui.QAction(self.tr('zoom'), self)
			self.connect(self.zoom, QtCore.SIGNAL('triggered()'), self.zoomItem)
			
			self.move = QtGui.QAction(self.tr('move'), self)
			self.connect(self.move, QtCore.SIGNAL('triggered()'), self.moveItem)
			 
			#~ popupmenu.addAction(self.delete)
			popupmenu.addAction(self.zoom)
			popupmenu.addAction(self.move)
			popupmenu.exec_(event.globalPos())
		
		self.showpopupmenu = False		
	
	#~ def deleteItem(self):
		#~ rubberbandselectedItem = self.sceneContainerPt.selectedItems()
		#~ if (len(rubberbandselectedItem) > 0):
			#~ self.emit(QtCore.SIGNAL("deleteitem(PyQt_PyObject)"),rubberbandselectedItem)
					
	def moveItem(self):
		self.setCursor(Qt.Qt.CrossCursor)
		
	def zoomItem(self):
		vTransform = self.viewportTransform()
		if( self.rubberbandWidth > 0  and self.rubberbandHeight >0):
			self.rubberbandlist = self.sceneContainerPt.items(self.startScenepos.x(),self.startScenepos.y(),self.rubberbandWidth,self.rubberbandHeight, Qt.Qt.IntersectsItemShape)
			for unselectitem in self.rubberbandlist:
				if unselectitem.isSelected() == True:
					unselectitem.setSelected(0)
	
			for items in (qgraphicsitem for qgraphicsitem in self.rubberbandlist if isinstance(qgraphicsitem,Textitem)):
				self.fitInView(self.startScenepos.x(),self.startScenepos.y(),self.rubberbandWidth,self.rubberbandHeight,Qt.Qt.KeepAspectRatio)
				if((self.matrix().m11()>=1.0)and(self.matrix().m22() >=1.0)):
					for item in ( Txtitem for Txtitem in self.sceneContainerPt.items() if isinstance (Txtitem, Textitem)):
						item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)
		else:
			self.rubberbandlist = self.sceneContainerPt.items(self.endScenepos.x(),self.endScenepos.y(),abs(self.rubberbandWidth),abs(self.rubberbandHeight), Qt.Qt.IntersectsItemShape)
			for unselectitem in self.rubberbandlist:
				if unselectitem.isSelected() == True:
					unselectitem.setSelected(0)
	
			for items in (qgraphicsitem for qgraphicsitem in self.rubberbandlist if isinstance(qgraphicsitem,Textitem)):
				self.fitInView(self.endScenepos.x(),self.endScenepos.y(),abs(self.rubberbandWidth),abs(self.rubberbandHeight),Qt.Qt.KeepAspectRatio)
				if((self.matrix().m11()>=1.0)and(self.matrix().m22() >=1.0)):
					for item in ( Txtitem for Txtitem in self.sceneContainerPt.items() if isinstance (Txtitem, Textitem)):
						item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)		
		self.rubberBandactive = False
	
class Widgetvisibility(Exception):pass

class LayoutWidget(QtGui.QWidget):
	def __init__(self,size,modelpath,parent=None):
		QtGui.QWidget.__init__(self,parent)
		grid = QtGui.QGridLayout()
		self.setLayout(grid)
		self.setWindowTitle('KineticsLayout')		
		self.availableSize = size
		self.sceneContainer = QtGui.QGraphicsScene(self)
		self.sceneContainer.setBackgroundBrush(QtGui.QColor(230,230,219,120))
		fnt = QtGui.QFont("Helvetica",16)
		self.modelPath = modelpath
		#moose.loadModel('../Anno_acc15.g','/foo')
		self.cmptmolDict = {}
		self.setupCompt(self.modelPath,self.cmptmolDict)
		
		#Compartment and its items
		#for c,items in self.cmptmolDict.items(): print c,items
		
		self.type = ['ZombieReac','ZombieEnz','ZombieMMenz','ZombieSumFunc']
		self.connectDict = {}
		
		self.setupItem(self.modelPath,self.type,self.connectDict)
		
		# zombieobject and its sub,prd,sumtotal
		#for k,v in self.connectDict.items(): print k,v
		
		self.qGraCompt = {}	
		self.moosetext_dict = {}
		
		self.border = 10
		
		#colorMap of kinetic kit
		self.colorMap = ((248,0,255),(240,0,255),(232,0,255),(224,0,255),(216,0,255),(208,0,255),(200,0,255),(192,0,255),(184,0,255),(176,0,255),(168,0,255),(160,0,255),(152,0,255),(144,0,255),(136,0,255),(128,0,255),(120,0,255),(112,0,255),(104,0,255),(96,0,255),(88,0,255),(80,0,255),(72,0,255),(64,0,255),(56,0,255),(48,0,255),(40,0,255),(32,0,255),(24,0,255),(16,0,255),(8,0,255),(0,0,255),(0,8,248),(0,16,240),(0,24,232),(0,32,224),(0,40,216),(0,48,208),(0,56,200),(0,64,192),(0,72,184),(0,80,176),(0,88,168),(0,96,160),(0,104,152),(0,112,144),(0,120,136),(0,128,128),(0,136,120),(0,144,112),(0,152,104),(0,160,96),(0,168,88),(0,176,80),(0,184,72),(0,192,64),(0,200,56),(0,208,48),(0,216,40),(0,224,32),(0,232,24),(0,240,16),(0,248,8),(0,255,0),(8,255,0),(16,255,0),(24,255,0),(32,255,0),(40,255,0),(48,255,0),(56,255,0),(64,255,0),(72,255,0),(80,255,0),(88,255,0),(96,255,0),(104,255,0),(112,255,0),(120,255,0),(128,255,0),(136,255,0),(144,255,0),(152,255,0),(160,255,0),(168,255,0),(176,255,0),(184,255,0),(192,255,0),(200,255,0),(208,255,0),(216,255,0),(224,255,0),(232,255,0),(240,255,0),(248,255,0),(255,255,0),(255,248,0),(255,240,0),(255,232,0),(255,224,0),(255,216,0),(255,208,0),(255,200,0),(255,192,0),(255,184,0),(255,176,0),(255,168,0),(255,160,0),(255,152,0),(255,144,0),(255,136,0),(255,128,0),(255,120,0),(255,112,0),(255,104,0),(255,96,0),(255,88,0),(255,80,0),(255,72,0),(255,64,0),(255,56,0),(255,48,0),(255,40,0),(255,32,0),(255,24,0),(255,16,0),(255,8,0),(255,0,0))
		
		#This is check which version of kkit, b'cos anything below kkit8 didn't had xyz co-ordinates
		allZero = "True"
		for compt,itemList in self.cmptmolDict.items():
			for items in (items for items in itemList if len(items) != 0):
				for item in items:
					iteminfo = item.path+'/info'
					x = float(Annotator(iteminfo).getField('x'))
					y = float(Annotator(iteminfo).getField('y'))
					
					if x != 0.0 or y != 0.0:
						allZero = False
						break
		
		if allZero:
			msgBox = QtGui.QMessageBox()
			msgBox.setText("The Layout module works for kkit version 8 or higher.")
			msgBox.setStandardButtons(QtGui.QMessageBox.Ok)
			msgBox.exec_()
			raise Widgetvisibility()
	
		else:
			fnt = QtGui.QFont("Helvetica",9)
			for compt,itemlist in self.cmptmolDict.items():
					for items in (items for items in itemlist if len(items) != 0):
						for item in items:
							pItem = Textitem(self,item)
							pItem.setFont(fnt)  
							iteminfo = item.path+'/info'
							x = float(Annotator(iteminfo).getField('x'))
							y = float(Annotator(iteminfo).getField('y'))
							pItem.setPos(x,y)
							#moosetext_dict[] is created to get the text's sceneBoundingRect
							self.moosetext_dict[item.id_] = pItem
							
			# dict for compartment and reactitem
			#for k,v in self.qGraCompt.items(): print k.id_,v
			
		#Calculate the scaling factor for cordinates		
		self.scale_Cord = int(self.cordTransform(self.cmptmolDict,self.moosetext_dict))		
		
		#Adding moose Object to scene and then adding to scene to view
		for compt,itemlist in self.cmptmolDict.items():
					self.create_compt(compt)
					compt_ref = self.qGraCompt[compt]
					for items in (items for items in itemlist if len(items) != 0):
						for item in items:
							pItem = Textitem(compt_ref,item)
							pItem.setFont(fnt)  
							iteminfo = item.path+'/info'
							x = float(Annotator(iteminfo).getField('x'))*(self.scale_Cord)
							y = float(Annotator(iteminfo).getField('y'))*(-self.scale_Cord)
							pItem.setPos(x,y)
							textColor = Annotator(iteminfo).getField('textColor')
							textBgcolor = Annotator(iteminfo).getField('color')
							self.connect(pItem, QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"), self.emitItemtoEditor)
							pItem.positionChange.connect(self.positionChange)
							self.moosetext_dict[item.id_] = pItem
							if((item.className =='ZombieEnz') or (item.className =='ZombieMMenz')):	
								iteminfo = moose.element(item.parent).path+'/info'
								textParcolor = Annotator(iteminfo).getField('color')
								self.colorCheck(pItem,item,textParcolor,"foreground")
								textbgcolor = "<html><body bgcolor='black'>"+item.name+"</body></html>"
								pItem.setHtml(QtCore.QString(textbgcolor))	
							
							else:
								if(textColor==textBgcolor):
									textBgcolor="white"
								self.colorCheck(pItem,item,textColor,"foreground")
								self.colorCheck(pItem,item,textBgcolor,"background")
								
		for k, v in self.qGraCompt.items():
				rectcompt = v.childrenBoundingRect()
				v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))
				v.setPen( QtGui.QPen( Qt.QColor(66,66,66,100),self.border,QtCore.Qt.SolidLine,QtCore.Qt.RoundCap,QtCore.Qt.RoundJoin ) )
		
		#connecting substrate,product to reaction and Enzyme
		self.lineItem_dict = {}
		self.object2line = {}
		for inn,out in self.connectDict.items():
			foor = len(filter(lambda x:isinstance(x,list), out))
			if (foor != 0):
				for items in (items for items in out[0] ):
					src = ""
					des = ""
					src = self.moosetext_dict[inn.id_]
					des = self.moosetext_dict[items.id_]
					self.lineCord(src,des,inn)
				for items in (items for items in out[1] ):
					src = ""
					des = ""
					des = self.moosetext_dict[inn.id_]
					src = self.moosetext_dict[items.id_]
					self.lineCord(src,des,inn)
			else:
				for items in (items for items in out ):
					src = ""
					des = ""
					src = self.moosetext_dict[inn.id_]
					des = self.moosetext_dict[items.id_]
					self.lineCord(src,des,inn)
					
		self.view = Graphicalview(self.sceneContainer,self.border)
		#~ self.view.connect(self.view,QtCore.SIGNAL("deleteitem(PyQt_PyObject)"),self.deleteArrow)
		#~ #changing from sceneRect to itemsboundingRect for windows compatibility as in windows sceneRect is not getting updated
		#~ self.view.fitInView(self.sceneContainer.sceneRect().x()-10,self.sceneContainer.sceneRect().y()-10,self.sceneContainer.sceneRect().width()+20,self.sceneContainer.sceneRect().height()+20,Qt.Qt.IgnoreAspectRatio)
		self.view.fitInView(self.sceneContainer.itemsBoundingRect().x()-10,self.sceneContainer.itemsBoundingRect().y()-10,self.sceneContainer.itemsBoundingRect().width()+20,self.sceneContainer.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
		self.view.centerOn(self.sceneContainer.itemsBoundingRect().center())
		grid.addWidget(self.view,0,0)					
		
	# setting up compartment and its items	
	def setupCompt(self,moosePath,comptDict):
		CPath = moosePath+'/##[TYPE=MeshEntry]'
		for meshEnt in wildcardFind(CPath):
			molList = []
			reList = []
			for mitem in Neutral(meshEnt).neighborDict['remesh']:
				if ( (mitem.className != 'GslIntegrator') and (element(mitem.parent).className != 'ZombieEnz') ):
					molList.append(mitem)
			for reitem in Neutral(meshEnt).neighborDict['remeshReacs']:
					reList.append(reitem)
			comptDict[meshEnt] = molList,reList
	
	def setupItem(self,moosePath,searObject,cntDict):
			for zombieObj in searObject:
				path = moosePath+'/##[TYPE='+zombieObj+']'
				if zombieObj != 'ZombieSumFunc':
					for items in wildcardFind(path):
						sublist = []
						prdlist = []
						for sub in items.getNeighbors('sub'): sublist.append(sub)
						for prd in items.getNeighbors('prd'): prdlist.append(prd)
						if zombieObj == 'ZombieEnz':
							for enzpar in items.getNeighbors('toZombieEnz'):
								sublist.append(enzpar)
						cntDict[items] = sublist,prdlist
				else:
					#ZombieSumFunc adding inputs
					for items in wildcardFind(path):
						inputlist = []
						outputlist = []
						funplist = []
						nfunplist = []
						for inpt in items.getNeighbors('input'): inputlist.append(inpt)
						for zfun in items.getNeighbors('output'): funplist.append(zfun)
						for i in funplist: nfunplist.append(i.id_)
						nfunplist = list(set(nfunplist))
						
						if(len(nfunplist) > 1): print "SumFunPool has multiple Funpool"
						else:
							for el in funplist:
								if(el.id_ == nfunplist[0]):
									cntDict[el] = inputlist
									break
		
	#Checking and creating compartment
	def create_compt(self,key):
		compt = Neutral(key.parent).name
		self.new_Compt = Rect_Compt(self,0,0,0,0,key)
		self.qGraCompt[key] = self.new_Compt
		self.new_Compt.setRect(10,10,10,10)
		self.sceneContainer.addItem(self.new_Compt)
		
	def cordTransform(self,comptlist,mooseItemdict):
		#here alpha is calculated to multipy the coordinates with, so that the each items spreads out
		alpha = 0
		alpha1 = 0
		for k,mrelist in comptlist.items():
			itemslist = mrelist[0]+mrelist[1]
			for m in range(len(itemslist)):
				for n in range(len(itemslist)):
					if(m != n):
						src = itemslist[m]
						self.pitemsrc = mooseItemdict[src.id_]
						src_Scenebounding = self.pitemsrc.sceneBoundingRect()
						srcX = src_Scenebounding.x()
						srcY = src_Scenebounding.y()
						srcW = src_Scenebounding.right()-src_Scenebounding.left()
						srcH = src_Scenebounding.bottom()-src_Scenebounding.top()
						
						des = itemslist[n]
						self.pitemdes = mooseItemdict[des.id_]
						des_Scenebounding = self.pitemdes.sceneBoundingRect()
						desX = des_Scenebounding.x()
						desY = des_Scenebounding.y()
						desW = des_Scenebounding.right()-des_Scenebounding.left()
						desH = des_Scenebounding.bottom()-des_Scenebounding.top()
						
						t = src_Scenebounding.intersects(des_Scenebounding)
						if t:
							sfx = 0
							sfy = 0
							if((desX - srcX)!= 0):	sfx = ( float(srcW)/abs(desX-srcX))
							if((desY - srcY)!= 0):	sfy = ( float(srcH)/abs(desY-srcY))
							if((sfx != 0) and (sfy != 0)):
								if( sfx < sfy):		alpha = sfx
								elif (sfy < sfx):	alpha = sfy
								else:			alpha = 0
							elif (sfx == 0): alpha = sfy
							elif (sfy == 0): alpha = sfx
							else:		 alpha =0
						else:
							pass
						if(alpha1 < alpha): alpha1 = alpha
						
					else: pass
			alpha1=alpha1+1
			return(alpha1)

	#color map for kinetic kit
	def colorCheck(self,pItem,item,textColor,fgbg_color):
		if(textColor == "<blank-string>"): textColor = "green"		
		if textColor.isdigit():
			tc = int(textColor)
			tc = (tc * 2 )
			r,g,b = self.colorMap[tc]
			if(fgbg_color == 'foreground'):
				pItem.setDefaultTextColor(QtGui.QColor(r,g,b))
			elif(fgbg_color == 'background'):
				hexchars = "0123456789ABCDEF"
				hexno = "#" + hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16]	
				textbgcolor = "<html><body bgcolor="+hexno+">"+item.name+"</body></html>"
				pItem.setHtml(QtCore.QString(textbgcolor))
		else:	
			if(fgbg_color == 'foreground'):
				pItem.setDefaultTextColor(QtGui.QColor(textColor))
			elif(fgbg_color == 'background'):
				textbgcolor = "<html><body bgcolor='"+textColor+"'>"+item.name+"</body></html>"
				pItem.setHtml(QtCore.QString(textbgcolor))
	
	# Calculating line distance
	def lineCord(self,src,des,source):
	
		if( (src == "") & (des == "") ):
			print "Source or destination is missing or incorrect"
		
		else:
			srcdes_list= [src,des]
			arrow = self.calArrow(src,des)
			if(source.className == "ZombieReac"):
				qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.green, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
				
			elif( (source.className == "ZombieEnz") or (source.className == "ZombieMMenz")):
				qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.red, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
		
			
			elif( (source.className == "ZombiePool") or (source.className == "ZombieFuncPool")):
				qgLineitem = self.sceneContainer.addPolygon(arrow,QtGui.QPen(QtCore.Qt.blue, 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
		
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
									
	def calArrow(self,src,des):
		sX = src.sceneBoundingRect().x()
		sY = src.sceneBoundingRect().y()		
		sw = src.sceneBoundingRect().right() -src.sceneBoundingRect().left()
		sh = src.sceneBoundingRect().bottom() -src.sceneBoundingRect().top()
		
		dX = des.sceneBoundingRect().x()
		dY = des.sceneBoundingRect().y()			
		dw = des.sceneBoundingRect().right() -des.sceneBoundingRect().left()
		dh = des.sceneBoundingRect().bottom() -des.sceneBoundingRect().top()
		
		
		#Here there is external boundary created for each textitem 
		#	1. for checking if there is overLap
		#	2. The start line and arrow head ends to this outer boundary
				
		srcRect = QtCore.QRectF(sX-5,sY-5,sw+10,sh+10)
		desRect = QtCore.QRectF(dX-5,dY-5,dw+10,dh+10)
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
			arrow.append(QtCore.QPointF(lineSrcpoint.x(),lineSrcpoint.y()))
			arrow.append(QtCore.QPointF(lineDespoint.x(),lineDespoint.y()))

			#Arrow head is drawned if the distance between src and des line is >8 just for clean appeareance
			if(abs(lineSrcpoint.x()-lineDespoint.x()) > 8 or abs(lineSrcpoint.y()-lineDespoint.y())>8):
				#Arrow head for Source is calculated
				degree = 60
				srcXArr1,srcYArr1= self.arrowHead(srcAngle,degree,lineSrcpoint)
				degree = 120
				srcXArr2,srcYArr2 = self.arrowHead(srcAngle,degree,lineSrcpoint)
				
				arrow.append(QtCore.QPointF(lineSrcpoint.x(),lineSrcpoint.y()))
				arrow.append(QtCore.QPointF(srcXArr1,srcYArr1))
				arrow.append(QtCore.QPointF(lineSrcpoint.x(),lineSrcpoint.y()))
				arrow.append(QtCore.QPointF(srcXArr2,srcYArr2))
			
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
	
	def positionChange(self,mooseObject):
		#If the item position changes, the corresponding arrow's are claculated
		if(isinstance(mooseObject, Textitem)):
				self.updatearrow(mooseObject)
		else:
			for k, v in self.Compt_dict.items():
					for rectChilditem in v.childItems():
						self.updatearrow(rectChilditem)
							
	def emitItemtoEditor(self,mooseObject):
		self.emit(QtCore.SIGNAL("itemDoubleClicked(PyQt_PyObject)"), mooseObject)
	
	def updatearrow(self,mooseObject):
		listItem = []
		for listItem in (v for k,v in self.object2line.items() if k.mooseObj_.id_ == mooseObject.mooseObj_.id_ ):
			if len(listItem):
				for ql,va in listItem:
					srcdes = self.lineItem_dict[ql]
					arrow = self.calArrow(srcdes[0],srcdes[1])
					ql.setPolygon(arrow)
			break

	def updateItemSlot(self, mooseObject):
		#In this case if the name is updated from the keyboard both in mooseobj and gui gets updation
		changedItem = ''
		for changedItem in (item for item in self.sceneContainer.items() if isinstance(item, Textitem) and mooseObject.id_ == item.mooseObj_.id_):
			break
		
		if isinstance(changedItem,Textitem):
			changedItem.updateSlot()
			self.positionChange(changedItem.mooseObj_)
		
	def keyPressEvent(self,event):
		#For Zooming
		#~ for item in self.sceneContainer.items():
			#~ if isinstance (item, Textitem):
				#~ print "self.view.matrix().m11()",self.view.matrix().m11(),self.view.matrix().m22()
				#~ 
				#~ if((self.view.matrix().m11()<=1.0)and(self.view.matrix().m22() <=1.0)):
					#~ item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, True)
				#~ else:
					#~ item.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)
					#~ 
			#~ elif isinstance(item,Rect_Compt):
					#~ pass
					
		key = event.key()
		
		if key == QtCore.Qt.Key_A:
			#~ self.view.resetMatrix()
			self.view.fitInView(self.sceneContainer.sceneRect().x()-10,self.sceneContainer.sceneRect().y()-10,self.sceneContainer.sceneRect().width()+20,self.sceneContainer.sceneRect().height()+20,Qt.Qt.IgnoreAspectRatio)
	
		elif (key == 46 or key == 62):
			self.view.scale(1.1,1.1)
			
		elif (key == 44 or key == 60):	
			self.view.scale(1/1.1,1/1.1)				
			
	#~ def deleteArrow(self,items):
		#~ compartmentlist = []
		#~ for mooseObject in items:
			#~ deleteline = []
			#~ for deleteline in (v for k,v in self.object2line.items() if k.mooseObj_.id == mooseObject.mooseObj_.id ):
				#~ if len(deleteline):
					#~ #All the line associated with graphicstext item is removed
					#~ for line,va in deleteline:
						#~ if self.deleteitemdict.has_key(line) == False:
							#~ self.sceneContainer.removeItem(line)
							#~ self.deleteitemdict[line] = self.lineItem_dict[line]
				#~ #Qgraphicstextitem is removed			
				#~ self.sceneContainer.removeItem(mooseObject)			
				#~ break
			#~ 
			#~ if mooseObject.mooseObj_.className == 'KinCompt':
				#~ if compartmentlist:
					#~ compartmentlist.append(mooseObject)
				#~ else:	
					#~ compartmentlist.insert(0,mooseObject)
		#~ 
		#~ for kingroup in compartmentlist:
				#~ #qgraphicsrectitem is empty then rectitem is also deleted
				#~ if len(kingroup.childItems()) == 0:
					#~ self.sceneContainer.removeItem(kingroup)
				#~ else:
					#~ self.deleteArrow(kingroup.childItems())
					#~ self.sceneContainer.removeItem(kingroup)

if __name__ == "__main__":
	#app = QtGui.QApplication([])
	size = 600
	dt = LayoutWidget(size)
	#dt.show()
	#sys.exit(app.exec_())

