# mgui.py --- 
# 
# Filename: mgui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Nov 12 09:38:09 2012 (+0530)
# Version: 
# Last-Updated: Fri Feb  8 15:56:50 2013 (+0530)
#           By: subha
#     Update #: 516
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
import inspect
import sys
sys.path.append('../python')
import os
from PyQt4 import QtGui,QtCore,Qt
import config
import mplugin
import moose
from mload import loadFile

# This maps model subtypes to corresponding plugin names. Should be
# moved to a separate property file perhaps
subtype_plugin_map = {
    'genesis/kkit': 'kkit'
}

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

    the view of the plugin can also provide its own list of
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
        self._loadedPlugins = {}
        self.quitAction = QtGui.QAction('Quit', self)
        self.connect(self.quitAction, QtCore.SIGNAL('triggered()'), self.quit)
        self.setPlugin('default', '/')        
	#self.setPlugin('kkit','/kho')

    def quit(self):
        QtGui.qApp.closeAllWindows()        
    
    def getPluginNames(self):
        """Return pluginNames attribute or create it by retrieving
        available plugin names from plugin/list.txt file.

        """
        if self.pluginNames is None:
            with open(os.path.join(config.MOOSE_GUI_DIR,
                                   'plugins', 
                                   'list.txt')) as lfile:
                self.pluginNames = [line.strip() for line in lfile]
                self.pluginNames = [name for name in self.pluginNames if name]
        return self.pluginNames

    def loadPluginModule(self, name, re=False):
        """Load a plugin module by name.

        First check if the plugin is already loaded. If so return the
        existing one. Otherwise, search load the plugin as a python
        module from {MOOSE_GUI_DIRECTORY}/plugins directory.

        If re is True, the plugin is reloaded.
        """
        if (not re) and name in sys.modules:
            return sys.modules[name]
        fp, pathname, description = imp.find_module(name, [config.MOOSE_PLUGIN_DIR])
        try:
            module = imp.load_module(name, fp, pathname, description)
        finally:
            if fp:
                fp.close()
        return module

    def loadPluginClass(self, name, re=False):        
        """Load the plugin class from a plugin module.
        
        A plugin module should have only one subclass of
        MoosePluginBase. Otherwise the first such class found will be
        loaded.
        """
        try:
            return self._loadedPlugins[name]
        except KeyError:
            pluginModule = self.loadPluginModule(name, re=re)
            for classname, classobj in inspect.getmembers(pluginModule, inspect.isclass):
                if issubclass(classobj, mplugin.MoosePluginBase):
                    self._loadedPlugins[name] = classobj
                    return self._loadedPlugins[name]
        raise Exception('No plugin with name: %s' % (name))

    def setPlugin(self, name, root='/'):
        """Set the current plugin to use.

        This -

        1. sets the `plugin` attribute.

        2. updates menus by clearing and reinstating menus including
        anything provided by the plugin.

        3. sets the current view  to the plugins editor view.
        """
        print 'Setting plugin'
        self.plugin = self.loadPluginClass(str(name))(str(root), self)
        self.updateMenus()
        self.setCurrentView(self.plugin.getEditorView())
        print self.plugin

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
        """Clear the menubar and reinstate the basic menus.  Go
        through the menus provided by current plugin and add those to
        menubar.

        If a menu provided by a plugin has same name as one of the
        core menus, the menu items provided by the plugin are appended
        to the existing menu after a separator.

        """
        self.menuBar().clear()
        self.menuBar().addMenu(self.getFileMenu())
        self.menuBar().addMenu(self.getEditMenu())
        self.menuBar().addMenu(self.getViewMenu())
        self.menuBar().addMenu(self.getPluginsMenu())
        self.menuBar().addMenu(self.getHelpMenu())
        for menu in self.plugin.getMenus():
            if not self.updateExistingMenu(menu):
                self.menuBar().addMenu(menu)

    def setCurrentView(self, view):
        """Set current view to a particular one: options are 'editor',
        'plot', 'run'. A plugin can provide more views if necessary.
        """
        self.plugin.setCurrentView(view)
        self.setCentralWidget(self.plugin.getCurrentView().getCentralWidget())
        for menu in self.plugin.getCurrentView().getMenus():
            if not self.updateExistingMenu(menu):
                self.menuBar().addMenu(menu)

    def getFileMenu(self):
        if self.fileMenu is None:
            self.fileMenu = QtGui.QMenu('File')
        else:
            self.fileMenu.clear()
        if not hasattr(self, 'loadModelAction'):
            self.loadModelAction = QtGui.QAction('Load model', self)
            self.connect(self.loadModelAction, QtCore.SIGNAL('triggered()'), self.loadModelDialogSlot)
        self.fileMenu.addAction(self.loadModelAction)
        # self.fileMenu.addAction(self.plugin.getSaveAction())
        self.fileMenu.addAction(self.quitAction)
        return self.fileMenu

    def getEditMenu(self):
        if self.editMenu is None:
            self.editMenu = QtGui.QMenu('Edit')
        else:
            self.editMenu.clear()
        self.editMenu.addActions(self.getEditActions())
        return self.editMenu

    def getPluginsMenu(self):
        """Populate plugins menu if it does not exist already."""
        if (not hasattr(self, 'pluginsMenu')) or (self.pluginsMenu is None):
            self.pluginsMenu = QtGui.QMenu('Plugins')
            mapper = QtCore.QSignalMapper(self)
            pluginsGroup = QtGui.QActionGroup(self)
            pluginsGroup.setExclusive(True)
            for pluginName in self.getPluginNames():                
                action = QtGui.QAction(pluginName, self)
                action.setObjectName(pluginName)
                action.setCheckable(True)
                mapper.setMapping(action, QtCore.QString(pluginName))
                self.connect(action, QtCore.SIGNAL('triggered()'), mapper, QtCore.SLOT('map()'))
                self.pluginsMenu.addAction(action)
                pluginsGroup.addAction(action)
            self.connect(mapper, QtCore.SIGNAL('mapped(const QString &)'), self.setPlugin)
        return self.pluginsMenu

    def getHelpMenu(self):
        if self.helpMenu is None:
            self.helpMenu = QtGui.QMenu('Help')
        else:
            self.helpMenu.clear()
        self.helpMenu.addActions(self.getHelpActions())        
        return self.helpMenu

    def getViewMenu(self):
        if (not hasattr(self, 'viewMenu')) or (self.viewMenu is None):
            self.viewMenu = QtGui.QMenu('View')
        else:
            self.viewMenu.clear()
        self.viewMenu.addActions(self.getViewActions())
        return self.viewMenu

    def getEditActions(self):
        if (not hasattr(self, 'editActions')) or (self.editActions is None):
            self.editActions = [] # TODO placeholder
        return self.editActions

    def getViewActions(self):
        if (not hasattr(self, 'viewActions')) or (self.viewActions is None):
            self.viewActions = [] # TODO placeholder
        return self.viewActions

    def getHelpActions(self):
        if (not hasattr(self, 'helpActions')) or (self.helpActions is None):
            self.actionAbout = QtGui.QAction('About MOOSE', self)
            self.connect(self.actionAbout, QtCore.SIGNAL('triggered()'), self.showAboutMoose)
            self.actionBuiltInDocumentation = QtGui.QAction('Built-in documentation', self)
            self.connect(self.actionBuiltInDocumentation, QtCore.SIGNAL('triggered()'), self.showBuiltInDocumentation)
            self.actionBug = QtGui.QAction('Report a bug', self)
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

    def loadModelDialogSlot(self):
        """Start a file dialog to choose a model file.

        Once the dialog succeeds, we should hand-over the duty of
        actual model loading to something else. Then refresh the
        views. Things to check from the user:

        1) The file type

        2) Target element

        3) Whether we should update the current window or start a new
        window.

        4) Plugin to use for displaying this model (can be automated
        by looking into the model file for a regular expression)

        """
        activeWindow = None # This to be used later to refresh the current widget with newly loaded model
        dialog = QtGui.QFileDialog(self, 
                                   self.tr('Load model from file'), 
                                   filter=self.tr('GENESIS (*.g);; GENESIS Prototype (*.p);; NeuroML/SBML (*.xml);; All files (*.*)'),
                                   options=QtGui.QFileDialog.ReadOnly)
        dialog.setNameFilterDetailsVisible(True)
        dialog.setFileMode(QtGui.QFileDialog.ExistingFile)
        targetPanel = QtGui.QFrame(dialog)
        targetLabel = QtGui.QLabel('Create model under')
        targetText = QtGui.QLineEdit('.')
        layout = QtGui.QVBoxLayout()
        layout.addWidget(targetLabel)
        layout.addWidget(targetText)
        targetPanel.setLayout(layout)
        dialog.layout().addWidget(targetPanel)
        self.connect(dialog, QtCore.SIGNAL('currentChanged(const QString &)'), 
                     lambda path:(targetText.setText(os.path.basename(str(path)).rpartition('.')[0])))
        if dialog.exec_():
            fileNames = dialog.selectedFiles()
            for fileName in fileNames:
                print 'Current plugin', self.plugin
                modelRoot = str(targetText.text())
                print 'modelroot', modelRoot
                ret = loadFile(str(fileName), modelRoot)
                try:
                    pluginName = subtype_plugin_map['%s/%s' % (ret['modeltype'], ret['subtype'])]
                except KeyError:
                    pluginName = 'default'
                self.setPlugin(pluginName, ret['model'].path)
                self.plugin.setCurrentView('editor')
                # self.plugin.getEditorView().getCentralWidget().setModelRoot()

if __name__ == '__main__':
    # create the GUI application
    app = QtGui.QApplication(sys.argv)
    QtGui.qApp = app
    icon = QtGui.QIcon(os.path.join(config.KEY_ICON_DIR,'moose_icon.png'))
    app.setWindowIcon(icon)
    # instantiate the main window
    #moose.loadModel('../Demos/Genesis_files/Kholodenko.g','/kho')
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
