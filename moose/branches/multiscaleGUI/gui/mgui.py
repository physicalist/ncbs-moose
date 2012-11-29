# mgui.py --- 
# 
# Filename: mgui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Nov 12 09:38:09 2012 (+0530)
# Version: 
# Last-Updated: Thu Nov 29 12:26:09 2012 (+0530)
#           By: subha
#     Update #: 241
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
import imp
import sys
import os
from PyQt4 import QtGui,QtCore,Qt
import config
import mplugin

class MWindow(QtGui.QMainWindow):
    """The main window for MOOSE GUI.

    This is the driver class that uses the mplugin API. mplugin based
    classes will provide the toolbar, plugin specific menu items and a
    set of panes to be displayed on the docks.

    1. Setting a plugin

       When a plugin is set as the current plugin, the view and the
       menus are updated.

    1.a) Updating menus: 

    the plugin can provide its own list of menus by implementing the
    function getMenus().

    the view widget of the plugin can also provide its own list of
    menus by implementing the function getMenus().

    the currentView provides a set of toolbars that are added to the
    main window.

    1.b) Updating views
    
    central widget is set to the currentView (a ViewBase instance) of
    the plugin.

    the currentView provides a set of panes that are inserted in the
    right dock area one by one.

    """
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.pluginNames = None
        self.plugin = None
        self.fileMenu = None
        self.editMenu = None
        self.helpMenu = None
        self.helpActions = None
        self.viewActions = None
        self.editActions = None                
        self.setPlugin('default')        
    
    def getPluginNames(self):
        if self.pluginNames is None:
            with open(os.path.join(config.MOOSE_GUI_DIR,
                                   'plugins', 
                                   'list.txt')) as lfile:
                self.pluginNames = [line.strip() for line in lfile]
        return self.pluginNames

    def loadPlugin(self, name, re=False):
        """Load a plugin by name.

        First check if the plugin is already loaded. If so return the
        existing one. Otherwise, search load the plugin as a python
        module from {MOOSE_GUI_DIRECTORY}/plugins directory.

        If re is True, the plugin is reloaded.
        """
        if not re:
            try:
                return sys.modules[name]
            except KeyError:
                pass
        fp, pathname, description = imp.find_module(name, config.MOOSE_PLUGIN_DIR)
        try:
            return imp.load_module(name, fp, pathname, description)
        finally:
            if fp:
                fp.close()

    def setPlugin(self, name):
        pluginModule = self.loadPlugin(name)
        plugin = None
        for obj in dir(pluginModule):
            if issubclass(obj, mplugin.MoosePlugin):
                plugin = obj(self)
                break
        if plugin is None:
            raise Exception('No plugin with name: %s' % (name))
        self.plugin.close()
        self.menuBar().clear()
        self.plugin = plugin
        self.updateMenus()
        self.setCurrentView(plugin.getDefaultView())

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

    def setCurrentView(self, view):
        self.plugin.setCurrentView(view)
        self.setCentralWidget(self.plugin.getCurrentView())
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
            self.connect(self.actionAbout, QtCore.SIGNAL('triggered()'), self.showAboutMoose)
            self.actionBuiltInDocumentation = QtGui.QAction('Built-in documentation')
            self.connect(self.actionBuiltInDocumentation, QtCore.SIGNAL('triggered()'), self.showBuiltInDocumentation)
            self.actionBug = QtGui.QAction('Report a bug')
            self.connect(self.actionBug, QtCore.SIGNAL('triggered()'), self.reportBug)
            self.helpActions = [self.actionAbout, self.actionBuiltInDocumentation, self.actionBug]
        return self.helpActions
        
    def showAboutMoose(self):
        with open(config.MOOSE_ABOUT_FILE, 'r') as aboutfile:
            QtGui.QMessageBox.about(self, 'About MOOSE', ''.join(aboutfile.readlines()))

    def showDocumentation(self, source):
        if not hasattr(self, 'documentationViewer'):
            self.documentationViewer = QtGui.QTextBrowser()
            self.documentationViewer.setOpenLinks(True)
            self.documentationViewer.setOpenExternalLinks(True)
            self.documentationViewer.setSearchPaths([config.settings[config.KEY_DOCS_DIR],
                                                     os.path.join(config.settings[config.KEY_DOCS_DIR], 'html'),
                                                     os.path.join(config.settings[config.KEY_DOCS_DIR], 'images')])
            self.documentationViewer.setMinimumSize(800, 480)
        self.documentationViewer.setSource(QtCore.QUrl(source))
        result = self.documentationViewer.loadResource(QtGui.QTextDocument.HtmlResource, self.documentationViewer.source())
        if not result.isValid():
            QtGui.QMessageBox.warning(self, 'Could not access documentation', 'The link %s could not be accessed' % (source))
            return
        self.documentationViewer.setWindowTitle(source)
        self.documentationViewer.reload()
        self.documentationViewer.setVisible(True)
    def reportBug(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(config.MOOSE_REPORT_BUG_URL))

    def showBuiltInDocumentation(self):
        self.showDocumentation('moosebuiltindocs.html')

if __name__ == '__main__':
    # create the GUI application
    app = QtGui.QApplication(sys.argv)
    icon = QtGui.QIcon(os.path.join(config.KEY_ICON_DIR,'moose_icon.png'))
    app.setWindowIcon(icon)
    # instantiate the main window
    mWindow =  MWindow()
    mWindow.setWindowState(QtCore.Qt.WindowMaximized)
    # show it
    mWindow.show()
    # start the Qt main loop execution, exiting from this script
    #http://code.google.com/p/subplot/source/browse/branches/mzViewer/PyMZViewer/mpl_custom_widget.py
    #http://eli.thegreenplace.net/files/prog_code/qt_mpl_bars.py.txt
    #http://lionel.textmalaysia.com/a-simple-tutorial-on-gui-programming-using-qt-designer-with-pyqt4.html
    #http://www.mail-archive.com/matplotlib-users@lists.sourceforge.net/msg13241.html
    # with the same return code of Qt application
    config.settings[config.KEY_FIRSTTIME] = 'False' # string not boolean
    sys.exit(app.exec_())
    

# 
# mgui.py ends here
