# mplugin.py --- 
# 
# Filename: mplugin.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Oct  2 17:25:41 2012 (+0530)
# Version: 
# Last-Updated: Wed Dec  5 16:56:53 2012 (+0530)
#           By: subha
#     Update #: 168
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This is to be the base class for all MOOSE GUI plugins.
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:

from PyQt4 import QtGui,QtCore,Qt

class MoosePluginBase(object):
    """Moose GUI plugin base class.

    A GUI plugin for MOOSE should extend MoosePlugin class. It has to
    implement the methods described here.

    """
    def __init__(self, root='/', mainwindow=None):
        """Create a plugin object whose model is the tree rooted at
        `root` and whose widgets will be displayed in `mainwindow`.
        
        """
        self._views = []
        self._menus = []
        self.mWindow = mainwindow
        self.modelRoot = root

    def getPreviousPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a smaller scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getNextPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a larger scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getAdjacentPlugins(self):
	"""Return a list of plugins that are valid transitions from this plugin"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getViews(self):
	"""Return the view widgets available from this plugin."""        
	raise NotImplementedError('method must be reimplemented in subclass')

    def getCurrentView(self):
	"""Return the current view of this plugin."""
	raise NotImplementedError('method must be reimplemented in subclass')    

    def setCurrentView(self, view):
        """Set current view (e.g., editor/plot/run).

        Return true if successful, otherwise return False.
        """
        if view == 'editor':
            self.currentView = self.getEditorView()
        elif view == 'plot':
            self.currentView = self.getPlotView()
        elif view == 'run':
            self.currentView = self.getRunView()
        else:
            return False
        return True

    def getMenus(self):
        return self._menus

    # def close(self):
    #     for view in self._views:
    #         view.close()

    def getEditorView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getPlotView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getRunView(self):
        raise NotImplementedError('method must be implemented in derived class')


class ViewBase(object):
    """Base class for each view: Editor, Plot, Run.

    A view is a mode in a of a plugin. Each view provides 

    a list of toolbars to be displayed on top.

    a list of widgets to be docked on the sides.

    a list of menus to be added to the menubar.

    a central widget to be displayed at the centre of the main window.

    """
    def __init__(self, plugin):
        self._menus = []
        self._toolPanes = []
        self._centralWidget = None
        self.plugin = plugin

    def getToolPanes(self):
        """Return a list of widgets to be displayed as dock widgets."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getToolBars(self):
        """Return list of toolbars for this view."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getMenus(self):
        """Return the menus for this view."""
        return self._menus

    def getPreferences(self):
        """Return a widget for setting preferences"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getCentralWidget(self):
        """Return a widget for central widget of mainwindow."""
        raise NotImplementedError('method must be reimplemented in subclass')

class EditorBase(ViewBase):
    """Base class for editor view.

    This is the default view of a plugin. It should essentially
    display a loaded model in an appropriate visual form.

    It is ultimately intended to allow editing of the model, but that
    is not a strict requirement.

    """
    def __init__(self, plugin):
        ViewBase.__init__(self, plugin)        

    def getToolPanes(self):
        if not self._toolPanes:
            self._toolPanes = [
                self.getLibraryPane(),
                self.getSelectionPane(),
                self.getOperationsPane()]

    def getLibraryPane(self):
        """TODO: display everything under library as a tree"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getSelectionPane(self):
        """TODO: provide a widget to apply selection rules"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsPane(self):
        """TODO: provide a widget to apply operations on selected
        elements."""
        raise NotImplementedError('method must be reimplemented in subclass')

class PlotBase(ViewBase):
    """Base class for plot configuration view.
    
    In each plugin, this should provide utility to setup the plotting
    of object fields. This is supposed to be used by intermediate
    users.
    """
    def __init__(self, *args):
        ViewBase.__init__(self, *args)

    def getSelectionPane(self):
        """TODO: provide a widget to apply selection rules"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsPane(self):
        """TODO: provide a widget to apply operations on selected
        elements."""
        raise NotImplementedError('method must be reimplemented in subclass')


class RunBase(ViewBase):
    """Base class for runtime view.

    When the simulation runs, this view displays the runtime
    visualization and controls for the simulation.
    """
    def __init__(self, *args):
        ViewBase.__init__(self, *args)


class EditorWidgetBase(QtGui.QWidget):
    """Base class for central widget displayed in editor view.

    The widget should display the model components in the tree rooted
    at `modelRoot` in appropriate visual representation.

    `updateModelView` function should do the actual creation and laying
    out of the visual objects.

    """
    def __init__(self, *args):
        QtGui.QWidget.__init__(self, *args)
        self.modelRoot = '/'
	
    def setModelRoot(self, path):
        """Set the root of the model tree to be displayed.

        This calls `updateModelView` which should update the scene to
        represent current model tree.

        This function can be a slot for connecting actions that should
        cause a change in modelRoot.

        """
        self.modelRoot = path
        self.updateModelView()

    def updateModelView(self):
        """Update view by going through the model.

        When model root is changed, this function is called. It should
        update the scene to represent the current model tree rooted at
        modelRoot.

        """
        raise NotImplementedError('must be implemented in derived class.')
    
    
# 
# mplugin.py ends here
