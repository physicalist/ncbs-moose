# mgui.py --- 
# 
# Filename: mgui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Nov 12 09:38:09 2012 (+0530)
# Version: 
# Last-Updated: Mon Nov 12 11:51:01 2012 (+0530)
#           By: subha
#     Update #: 18
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

    """
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.plugin = None

    def setPlugin(self, plugin):        
        self.plugin = plugin
        

# 
# mgui.py ends here
