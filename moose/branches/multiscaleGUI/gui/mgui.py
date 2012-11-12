# mgui.py --- 
# 
# Filename: mgui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Nov 12 09:38:09 2012 (+0530)
# Version: 
# Last-Updated: Mon Nov 12 16:32:01 2012 (+0530)
#           By: subha
#     Update #: 125
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# The gui driver
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
import config

class MWindow(QtGui.QMainWindow):
    """The main window for MOOSE GUI.

    This is the driver class that uses the mplugin API. mplugin based
    classes will provide the toolbar, plugin specific menu items and a
    set of panes to be displayed on the docks.

    1. Setting a plugin

       When a plugin is set as the current plugin, the view and the
       menus are updated.

    """
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.plugin = None
        self.fileMenu = None
        self.editMenu = None
        self.helpMenu = None
        self.helpActions = None
        self.viewActions = None
        self.editActions = None        

    def setPlugin(self, plugin):        
        self.menuBar().clear()
        self.plugin = plugin
        self.updateView()
        self.updateMenus()
        cwidget = self.plugin.getCurrentView()
        if cwidget is not None:
            self.setCentralWidget(cwidget)

    def updateExistingMenu(self, menu):
        """Check if a menu with same title
        already exists. If so, update the same and return
        True. Otherwise return False.

        """
        for existingMenu in self.menuBar().children():
            if menu.title() == existingMenu.title():
                existingMenu.addSeparator()
                existingMenu.addActions(menu.actions())
                return True
        return False
        
        
    def updateMenus(self):
        self.menuBar().clear()
        self.menuBar().addMenu(self.getFileMenu())
        self.menuBar().addMenu(self.getEditMenu())
        self.menuBar().addMenu(self.getViewMenu())
        self.menuBar().addMenu(self.getHelpMenu())
        for menu in self.plugin.getMenus():
            if not self.updateExistingMenu(menu):
                self.menuBar().addMenu(menu)
        for menu in self.plugin.getCurrentView().getMenus():
            if not self.updateExistingMenu(menu):
                self.menuBar().addMenu(menu)

    def getFileMenu(self):
        if self.fileMenu is None:
            self.fileMenu = QtGui.QMenu('File')
        else:
            self.fileMenu.clear()
        self.fileMenu.addAction(self.plugin.getLoadAction())
        self.fileMenu.addAction(self.plugin.getSaveAction())
        self.fileMenu.addAction(self.quitAction)

    def getEditMenu(self):
        if self.editMenu is None:
            self.editMenu = QtGui.QMenu('Edit')
        else:
            self.editMenu.clear()
        self.editMenu.addActions(self.getEditActions())
    
    def getHelpMenu(self):
        if self.helpMenu is None:
            self.helpMenu = QtGui.QMenu('Help')
        else:
            self.helpMenu.clear()
        self.helpMenu.addActions(self.getHelpActions())        

    def getViewMenu(self):
        if self.viewMenu is None:
            self.viewMenu = QtGui.QMenu('View')
        else:
            self.viewMenu.clear()
        self.viewMenu.addActions(self.getViewActions())

    def getEditActions(self):
        if self.editActions is None:
            self.editActions = [] # TODO placeholder
        return self.editActions

    def getViewActions(self):
        if self.viewActions is None:
            self.viewActions = [] # TODO placeholder
        return self.viewActions

    def getHelpActions(self):
        if self.helpActions is None:
            self.actionAbout = QtGui.QAction('About MOOSE')
            self.connect(self.aboutAction, QtCore.SIGNAL('triggered()')
            self.actionBuiltInDocumentation = QtGui.QAction('Built-in documentation')
            self.actionBug = QtGui.QAction('Report a bug')
            self.helpActions = [self.actionAbout, self.actionBuiltInDocumentation, self.actionBug]
        return self.helpActions
        

# 
# mgui.py ends here
