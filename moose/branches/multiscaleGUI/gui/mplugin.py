# mplugin.py --- 
# 
# Filename: mplugin.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Oct  2 17:25:41 2012 (+0530)
# Version: 
# Last-Updated: Tue Oct  2 17:58:51 2012 (+0530)
#           By: subha
#     Update #: 22
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

class MoosePlugin(object):
    """Moose GUI plugin base class"""
    def __init__(self):
	pass

    def getPreviousPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a smaller scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getNextPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a larger scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getAllAllowedPlugins(self):
	"""Return a list of plugins that are valid transitions from this plugin"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getEditorWidget(self):
	"""Returns the editor object for the current level"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getLibraryWidget(self):
	"""Returns the library widget for current level of detail"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsWidget(self):
	"""Return the widget with operations that can be applied on
	objects in current level"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getSelectionWidget(self):
	"""Return the widget with selection options that can be
	applied on objects in current level"""
	raise NotImplementedError('method must be reimplemented in subclass')
	

# 
# mplugin.py ends here
