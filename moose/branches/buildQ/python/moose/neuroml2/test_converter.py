# test_converter.py --- 
# 
# Filename: test_converter.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Apr 23 18:51:58 2013 (+0530)
# Version: 
# Last-Updated: Mon Apr 29 10:45:20 2013 (+0530)
#           By: subha
#     Update #: 187
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

import os
import numpy as np
import uuid
import unittest
import pylab
import moose
import converter
import neuroml
from neuroml.writers import NeuroMLWriter

outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tmp')
if not os.access(outdir, os.F_OK):
    print 'Creating output directory', outdir
    os.mkdir(outdir)

class TestConvertMorphology(unittest.TestCase):
    def setUp(self):
        self.test_id = uuid.uuid4()
        self.model_container = moose.Neutral('test%s' % (self.test_id))
        self.neuron = moose.Neuron('%s/cell' % (self.model_container.path))
        self.soma = moose.Compartment('%s/soma' % (self.neuron.path))
        self.soma.diameter = 20e-6
        self.soma.length = 0.0
        parent = self.soma
        comps = []
        for ii in range(100):
            comp = moose.Compartment('%s/comp_%d' % (self.neuron.path, ii))
            comp.diameter = 10e-6
            comp.length = 100e-6
            moose.connect(parent, 'raxial', comp, 'axial')
            comps.append(comp)
            parent = comp
    
    def test_convert_morphology(self):
        morph = converter.convert_morphology(self.neuron, positions='auto')
        cell = neuroml.Cell()
        cell.name = self.neuron.name
        cell.id = cell.name
        cell.morphology = morph
        doc = neuroml.NeuroMLDocument()
        doc.cells.append(cell)
        doc.id = 'TestNeuroMLDocument'
        fname = os.path.join(outdir, 'test_morphology_conversion.nml')        
        NeuroMLWriter.write(doc, fname)
        print 'Wrote', fname

class TestFindRateFn(unittest.TestCase):
    def setUp(self):
        self.vmin = -120e-3
        self.vmax = 40e-3
        self.vdivs = 640
        self.v_array = np.linspace(self.vmin, self.vmax, self.vdivs+1)
        self.v0_sigmoid = -38e-3
        self.k_sigmoid = 1/(-10e-3)
        self.a_sigmoid = 1.0
        self.v0_exp = -53.5e-3
        self.k_exp = 1/(-27e-3)
        self.a_exp = 2e3
        # A sigmoid function - from traub2005, NaF->m_inf
        self.sigmoid = self.a_sigmoid / (1.0 + np.exp((self.v_array - self.v0_sigmoid) * self.k_sigmoid))
        # An exponential function - from traub2005, KC->n_inf
        self.exp = self.a_exp * np.exp((self.v_array - self.v0_exp) * self.k_exp)        
        # A linoid function
        self.a_linoid, self.k_linoid, self.v0_linoid = -0.01*1e3, -1/10e-3, 10e-3
        # This is alpha_n from original Hodgkin-Huxley K channel.
        self.linoid = converter.linoid(self.v_array, self.a_linoid, self.k_linoid, self.v0_linoid)

    def test_sigmoid(self):
        fn, params = converter.find_ratefn(self.v_array, self.sigmoid)
        print 'Sigmoid params original:', self.a_sigmoid, self.k_sigmoid, self.v0_sigmoid, 'detected:', params
        self.assertEqual(converter.sigmoid, fn)
        errors = params - np.array([self.a_sigmoid, self.k_sigmoid, self.v0_sigmoid])
        for err in errors:
            self.assertAlmostEqual(err, 0.0)

    def test_exponential(self):
        fn, params = converter.find_ratefn(self.v_array, self.exp)
        print 'Exponential params original:', self.a_exp, self.k_exp, self.v0_exp, 'detected:', params
        fnval = converter.exponential(self.v_array, *params)
        self.assertEqual(converter.exponential, fn)
        rms_error = np.sqrt(np.sum((self.exp - fnval)**2))
        # pylab.plot(self.v_array, self.exp, 'b-')
        # pylab.plot(self.v_array, fnval, 'r-.') 
        # pylab.show()
        print rms_error, rms_error/max(self.exp)
        self.assertAlmostEqual(rms_error/max(self.exp), 0.0)

    def test_linoid(self):
        fn, params = converter.find_ratefn(self.v_array, self.linoid)
        print 'Linoid params original:', self.a_linoid, self.k_linoid, self.v0_linoid, 'detected:', params
        pylab.plot(self.v_array, self.linoid, 'r-')
        pylab.plot(self.v_array, fn(self.v_array, *params), 'k-.')
        pylab.show()
        self.assertEqual(converter.linoid, fn)
        errors = params - np.array((self.a_linoid, self.k_linoid, self.v0_linoid))
        for orig, err in zip((self.a_linoid, self.k_linoid, self.v0_linoid), errors):
            self.assertAlmostEqual(abs(err/orig), 0.0, places=2)

if __name__ == '__main__':
    unittest.main()
        


# 
# test_converter.py ends here
