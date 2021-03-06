# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'vizParasDialogue.ui'
#
# Created: Thu May 19 00:20:13 2011
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

#Author:Chaitanya CH
#FileName: vizParasDialogue.py

#This program is free software; you can redistribute it and/or
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

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(493, 574)
        self.label = QtGui.QLabel(Dialog)
        self.label.setGeometry(QtCore.QRect(230, 235, 67, 17))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setGeometry(QtCore.QRect(50, 240, 67, 17))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(Dialog)
        self.label_3.setGeometry(QtCore.QRect(100, 295, 111, 17))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_4 = QtGui.QLabel(Dialog)
        self.label_4.setGeometry(QtCore.QRect(280, 295, 121, 17))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.variable = QtGui.QLineEdit(Dialog)
        self.variable.setGeometry(QtCore.QRect(100, 235, 113, 27))
        self.variable.setObjectName(_fromUtf8("variable"))
        self.moosepath = QtGui.QLineEdit(Dialog)
        self.moosepath.setGeometry(QtCore.QRect(280, 235, 113, 27))
        self.moosepath.setObjectName(_fromUtf8("moosepath"))
        self.vizMinVal = QtGui.QLineEdit(Dialog)
        self.vizMinVal.setGeometry(QtCore.QRect(100, 315, 113, 27))
        self.vizMinVal.setToolTip(_fromUtf8(""))
        self.vizMinVal.setObjectName(_fromUtf8("vizMinVal"))
        self.vizMaxVal = QtGui.QLineEdit(Dialog)
        self.vizMaxVal.setGeometry(QtCore.QRect(280, 315, 113, 27))
        self.vizMaxVal.setToolTip(_fromUtf8(""))
        self.vizMaxVal.setObjectName(_fromUtf8("vizMaxVal"))
        self.label_5 = QtGui.QLabel(Dialog)
        self.label_5.setGeometry(QtCore.QRect(142, 370, 81, 17))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.acceptButton = QtGui.QPushButton(Dialog)
        self.acceptButton.setGeometry(QtCore.QRect(360, 530, 95, 27))
        self.acceptButton.setObjectName(_fromUtf8("acceptButton"))
        self.resetButton = QtGui.QPushButton(Dialog)
        self.resetButton.setGeometry(QtCore.QRect(210, 530, 95, 27))
        self.resetButton.setObjectName(_fromUtf8("resetButton"))
        self.label_6 = QtGui.QLabel(Dialog)
        self.label_6.setGeometry(QtCore.QRect(120, 260, 261, 31))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.mtree = MooseTreeWidget(Dialog)
        self.mtree.setGeometry(QtCore.QRect(30, 30, 191, 131))
        self.mtree.setObjectName(_fromUtf8("mtree"))
        self.vizCells = QtGui.QListWidget(Dialog)
        self.vizCells.setGeometry(QtCore.QRect(280, 30, 181, 131))
        self.vizCells.setObjectName(_fromUtf8("vizCells"))
        self.label_8 = QtGui.QLabel(Dialog)
        self.label_8.setGeometry(QtCore.QRect(30, 10, 101, 17))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_9 = QtGui.QLabel(Dialog)
        self.label_9.setGeometry(QtCore.QRect(280, 10, 121, 17))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.closeButton = QtGui.QPushButton(Dialog)
        self.closeButton.setGeometry(QtCore.QRect(50, 530, 95, 27))
        self.closeButton.setObjectName(_fromUtf8("closeButton"))
        self.addCellButton = QtGui.QToolButton(Dialog)
        self.addCellButton.setGeometry(QtCore.QRect(240, 30, 24, 25))
        self.addCellButton.setObjectName(_fromUtf8("addCellButton"))
        self.removeCellButton = QtGui.QToolButton(Dialog)
        self.removeCellButton.setGeometry(QtCore.QRect(240, 80, 24, 25))
        self.removeCellButton.setObjectName(_fromUtf8("removeCellButton"))
        self.allCellsButton = QtGui.QToolButton(Dialog)
        self.allCellsButton.setGeometry(QtCore.QRect(240, 130, 24, 25))
        self.allCellsButton.setObjectName(_fromUtf8("allCellsButton"))
        self.styleComboBox = QtGui.QComboBox(Dialog)
        self.styleComboBox.setGeometry(QtCore.QRect(100, 180, 111, 31))
        self.styleComboBox.setObjectName(_fromUtf8("styleComboBox"))
        self.styleComboBox.addItem(_fromUtf8(""))
        self.styleComboBox.addItem(_fromUtf8(""))
        self.styleComboBox.addItem(_fromUtf8(""))
        self.styleComboBox.addItem(_fromUtf8(""))
        self.label_10 = QtGui.QLabel(Dialog)
        self.label_10.setGeometry(QtCore.QRect(50, 190, 67, 17))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.colorMapComboBox = QtGui.QComboBox(Dialog)
        self.colorMapComboBox.setGeometry(QtCore.QRect(230, 360, 101, 31))
        self.colorMapComboBox.setObjectName(_fromUtf8("colorMapComboBox"))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.colorMapComboBox.addItem(_fromUtf8(""))
        self.specificCompartmentName = QtGui.QLineEdit(Dialog)
        self.specificCompartmentName.setEnabled(False)
        self.specificCompartmentName.setGeometry(QtCore.QRect(280, 182, 113, 27))
        self.specificCompartmentName.setObjectName(_fromUtf8("specificCompartmentName"))
        self.label_7 = QtGui.QLabel(Dialog)
        self.label_7.setEnabled(False)
        self.label_7.setGeometry(QtCore.QRect(230, 188, 67, 17))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_11 = QtGui.QLabel(Dialog)
        self.label_11.setEnabled(False)
        self.label_11.setGeometry(QtCore.QRect(230, 424, 67, 17))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.label_12 = QtGui.QLabel(Dialog)
        self.label_12.setEnabled(False)
        self.label_12.setGeometry(QtCore.QRect(50, 425, 67, 17))
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.label_13 = QtGui.QLabel(Dialog)
        self.label_13.setEnabled(False)
        self.label_13.setGeometry(QtCore.QRect(280, 455, 131, 17))
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.label_14 = QtGui.QLabel(Dialog)
        self.label_14.setEnabled(False)
        self.label_14.setGeometry(QtCore.QRect(100, 455, 121, 17))
        self.label_14.setObjectName(_fromUtf8("label_14"))
        self.vizMinVal_2 = QtGui.QLineEdit(Dialog)
        self.vizMinVal_2.setEnabled(False)
        self.vizMinVal_2.setGeometry(QtCore.QRect(100, 475, 113, 27))
        self.vizMinVal_2.setText(_fromUtf8(""))
        self.vizMinVal_2.setObjectName(_fromUtf8("vizMinVal_2"))
        self.vizMaxVal_2 = QtGui.QLineEdit(Dialog)
        self.vizMaxVal_2.setEnabled(False)
        self.vizMaxVal_2.setGeometry(QtCore.QRect(280, 475, 113, 27))
        self.vizMaxVal_2.setText(_fromUtf8(""))
        self.vizMaxVal_2.setObjectName(_fromUtf8("vizMaxVal_2"))
        self.variable_2 = QtGui.QLineEdit(Dialog)
        self.variable_2.setEnabled(False)
        self.variable_2.setGeometry(QtCore.QRect(100, 420, 113, 27))
        self.variable_2.setText(_fromUtf8(""))
        self.variable_2.setObjectName(_fromUtf8("variable_2"))
        self.moosepath_2 = QtGui.QLineEdit(Dialog)
        self.moosepath_2.setEnabled(False)
        self.moosepath_2.setGeometry(QtCore.QRect(280, 420, 113, 27))
        self.moosepath_2.setObjectName(_fromUtf8("moosepath_2"))

        self.retranslateUi(Dialog)
        self.styleComboBox.setCurrentIndex(2)
        QtCore.QObject.connect(self.closeButton, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.reject)
        QtCore.QMetaObject.connectSlotsByName(Dialog)
        Dialog.setTabOrder(self.styleComboBox, self.specificCompartmentName)
        Dialog.setTabOrder(self.specificCompartmentName, self.variable)
        Dialog.setTabOrder(self.variable, self.moosepath)
        Dialog.setTabOrder(self.moosepath, self.vizMinVal)
        Dialog.setTabOrder(self.vizMinVal, self.vizMaxVal)
        Dialog.setTabOrder(self.vizMaxVal, self.colorMapComboBox)
        Dialog.setTabOrder(self.colorMapComboBox, self.variable_2)
        Dialog.setTabOrder(self.variable_2, self.moosepath_2)
        Dialog.setTabOrder(self.moosepath_2, self.vizMinVal_2)
        Dialog.setTabOrder(self.vizMinVal_2, self.vizMaxVal_2)
        Dialog.setTabOrder(self.vizMaxVal_2, self.closeButton)
        Dialog.setTabOrder(self.closeButton, self.resetButton)
        Dialog.setTabOrder(self.resetButton, self.acceptButton)
        Dialog.setTabOrder(self.acceptButton, self.addCellButton)
        Dialog.setTabOrder(self.addCellButton, self.removeCellButton)
        Dialog.setTabOrder(self.removeCellButton, self.allCellsButton)
        Dialog.setTabOrder(self.allCellsButton, self.vizCells)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Visualization Parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Dialog", "Path", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Dialog", "Field*", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Dialog", "Minimum Value*", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Dialog", "Maximum Value*", None, QtGui.QApplication.UnicodeUTF8))
        self.variable.setToolTip(QtGui.QApplication.translate("Dialog", "Name of the field to be visualized (/Cell/Compartment/Field)", None, QtGui.QApplication.UnicodeUTF8))
        self.variable.setText(QtGui.QApplication.translate("Dialog", "Vm", None, QtGui.QApplication.UnicodeUTF8))
        self.moosepath.setToolTip(QtGui.QApplication.translate("Dialog", "Name of the moose path relative to compartment of the field (/Cell/Compartment/Path/Field)", None, QtGui.QApplication.UnicodeUTF8))
        self.vizMinVal.setText(QtGui.QApplication.translate("Dialog", "-0.1", None, QtGui.QApplication.UnicodeUTF8))
        self.vizMaxVal.setText(QtGui.QApplication.translate("Dialog", "0.07", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Dialog", "Color Map", None, QtGui.QApplication.UnicodeUTF8))
        self.acceptButton.setText(QtGui.QApplication.translate("Dialog", "OK", None, QtGui.QApplication.UnicodeUTF8))
        self.resetButton.setText(QtGui.QApplication.translate("Dialog", "Reset", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Dialog", "(path relative to compartment path)", None, QtGui.QApplication.UnicodeUTF8))
        self.mtree.setToolTip(QtGui.QApplication.translate("Dialog", "Select cells to visualize, double click to add cell OR select and click \'+\' button, click \'A\' button to draw all cells", None, QtGui.QApplication.UnicodeUTF8))
        self.vizCells.setToolTip(QtGui.QApplication.translate("Dialog", "List of cells to visualize", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Dialog", "Moose Tree", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Dialog", "Cells to visualize*", None, QtGui.QApplication.UnicodeUTF8))
        self.closeButton.setText(QtGui.QApplication.translate("Dialog", "Close", None, QtGui.QApplication.UnicodeUTF8))
        self.addCellButton.setToolTip(QtGui.QApplication.translate("Dialog", "Add selected cell to the visualization list", None, QtGui.QApplication.UnicodeUTF8))
        self.addCellButton.setText(QtGui.QApplication.translate("Dialog", "+", None, QtGui.QApplication.UnicodeUTF8))
        self.removeCellButton.setToolTip(QtGui.QApplication.translate("Dialog", "Remove selected cell from visualization", None, QtGui.QApplication.UnicodeUTF8))
        self.removeCellButton.setText(QtGui.QApplication.translate("Dialog", "-", None, QtGui.QApplication.UnicodeUTF8))
        self.allCellsButton.setToolTip(QtGui.QApplication.translate("Dialog", "Add all cells to the visualization list", None, QtGui.QApplication.UnicodeUTF8))
        self.allCellsButton.setText(QtGui.QApplication.translate("Dialog", "A", None, QtGui.QApplication.UnicodeUTF8))
        self.styleComboBox.setItemText(0, QtGui.QApplication.translate("Dialog", "Compartment Disk", None, QtGui.QApplication.UnicodeUTF8))
        self.styleComboBox.setItemText(1, QtGui.QApplication.translate("Dialog", "Ball & Stick", None, QtGui.QApplication.UnicodeUTF8))
        self.styleComboBox.setItemText(2, QtGui.QApplication.translate("Dialog", "True Dim", None, QtGui.QApplication.UnicodeUTF8))
        self.styleComboBox.setItemText(3, QtGui.QApplication.translate("Dialog", "Grid View", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("Dialog", "Style", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(0, QtGui.QApplication.translate("Dialog", "jet", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(1, QtGui.QApplication.translate("Dialog", "grey", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(2, QtGui.QApplication.translate("Dialog", "redhot", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(3, QtGui.QApplication.translate("Dialog", "fire", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(4, QtGui.QApplication.translate("Dialog", "greenfire", None, QtGui.QApplication.UnicodeUTF8))
        self.colorMapComboBox.setItemText(5, QtGui.QApplication.translate("Dialog", "heat", None, QtGui.QApplication.UnicodeUTF8))
        self.specificCompartmentName.setToolTip(QtGui.QApplication.translate("Dialog", "Compartment Name. Default is \'soma\'", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Dialog", "Name", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("Dialog", "Path2", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("Dialog", "Field2", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("Dialog", "Maximum Value2", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("Dialog", "Minimum Value2", None, QtGui.QApplication.UnicodeUTF8))
        self.variable_2.setToolTip(QtGui.QApplication.translate("Dialog", "Name of the field to be visualized as changing disk size (/Cell/Compartment/Field)", None, QtGui.QApplication.UnicodeUTF8))
        self.moosepath_2.setToolTip(QtGui.QApplication.translate("Dialog", "Name of the moose path relative to compartment of the field (/Cell/Compartment/Path/Field)", None, QtGui.QApplication.UnicodeUTF8))

from moosetree import MooseTreeWidget
