# config.py --- 
# 
# Filename: config.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Sat Feb 13 16:07:56 2010 (+0530)
# Version: 
# Last-Updated: Mon Feb 15 01:08:07 2010 (+0530)
#           By: Subhasis Ray
#     Update #: 100
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
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

GL_CLIENT_EXECUTABLE = '../../gl/src/glclient'
GL_COLORMAP_DIR = '../../gl/colormaps'
GL_COLORMAP_RAINBOW2 = 'rainbow2'
GL_COLORMAP_HOT = 'hot'
GL_COLORMAP_GREY = 'grey'
GL_COLORMAP_REDHOT = 'redhot'
GL_DEFAULT_COLORMAP = '../../gl/colormaps/rainbow2'
GL_PORT = '9999'

import logging
from PyQt4.Qt import Qt
from PyQt4 import QtGui, QtCore

settings = None

# KEY_STATE_FILE = 'statefile'
KEY_GL_COLORMAP = 'glclient/colormap'
KEY_GL_PORT = 'glclient/port'
KEY_GL_CLIENT_EXECUTABLE = 'glclient/executable'

KEY_WINDOW_GEOMETRY = 'main/geometry'
KEY_WINDOW_LAYOUT = 'main/layout'

def get_settings():
    '''Initializes the QSettings for the application and returns it.'''
    global settings
    if not settings:
	QtCore.QCoreApplication.setOrganizationName('NCBS')
	QtCore.QCoreApplication.setOrganizationDomain('ncbs.res.in')
	QtCore.QCoreApplication.setApplicationName('MOOSE')
    settings = QtCore.QSettings()
    return settings

LOG_FILENAME = 'moose.log'
LOG_LEVEL = logging.DEBUG
logging.basicConfig(filename=LOG_FILENAME, level=LOG_LEVEL, filemode='w')
LOGGER = logging.getLogger('moose')
BENCHMARK_LOGGER = logging.getLogger('moose.benchmark')
BENCHMARK_LOGGER.setLevel(logging.INFO)

# 
# config.py ends here
