import sys
sys.path.append('plugins')
from default import *

from mplugin import *
class KkitPlugin(MoosePlugin):
    """Default plugin for MOOSE GUI"""
    def __init__(self, *args):
        #print args
        MoosePlugin.__init__(self, *args)

    def getPreviousPlugin(self):
        return None

    def getNextPlugin(self):
        return None

    def getAdjacentPlugins(self):
        return []

    def getViews(self):
        return self._views

    def getCurrentView(self):
        return self.currentView

    def getEditorView(self):
        if not hasattr(self, 'editorView'):
            self.editorView = KkitEditorView(self)
            self.currentView = self.editorView
        return self.editorView


class KkitEditorView(MooseEditorView):
    """Default editor.

    TODO: Implementation - display moose element tree as a tree or as
    boxes inside boxes

    """
    def __init__(self, plugin):
        MooseEditorView.__init__(self, plugin)

    def getToolPanes(self):
        return super(KkitEditorView, self).getToolPanes()

    def getLibraryPane(self):
        return super(KkitEditorView, self).getLibraryPane()

    def getOperationsWidget(self):
        return super(KkitEditorView, self).getOperationsPane()

    def getCentralWidget(self):
        if self._centralWidget is None:
            self._centralWidget = KineticsWidget()
            self._centralWidget.setModelRoot(self.plugin.modelRoot)
        return self._centralWidget

class  KineticsWidget(EditorWidgetBase):
    def __init__(self, *args):
        #QtGui.QWidget.__init__(self,parent)
	EditorWidgetBase.__init__(self, *args)

        #print "KKIT plugin",self.modelRoot
        
    def updateModelView(self):
        print "Here in kkit plugin overridden updatemodelview from EditorWidgetBase and  modelroot path",self.modelRoot
