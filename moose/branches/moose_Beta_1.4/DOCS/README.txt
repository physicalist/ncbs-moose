/*******************************************************************
 * File:            README
 * Description:     This file contains basic information regarding
 *		    MOOSE, the Multiscale Object-Oriented Simulation
 *		    Environment.
 *
 *		    This file is copied from the README for release 
 *		    moose-Beta 1.4.0. 
 * 
 * Created:         2010-11-17 18:44:03 (+0530)
 ********************************************************************/

Look for documentation in the following locations (in current
directory):

1. README - (this file) contains basic information about MOOSE.

2. INSTALL - contains detailed instruction on configuring and building
	     moose from the source code.

3. DOCS - a directory containining assorted documentation.

MOOSE is the base and numerical core for large, detailed simulations including
Computational Neuroscience and Systems Biology. 


New in this release:
A PyQt-based graphical interface for running basic simulations and tutorials.
	This is now the default 'face' of MOOSE.
Gillespie solver for chemical kinetic simulations.
SBML support for writing kinetic models, partial support for reading.
MPI support for parallel neuronal simulations, and interface to MUSIC.
Upgrades to neuronal solver.

Quick start (Linux):

Requirements:
	python2.5(>=2.5),python-qt4(>=4.5),python-numpy(>=1.0.4),
	python-qwt5-qt4

DEMOS:
  - Upon installation Demos,Tests and documentation go into /usr/share/doc/ 
    and also your Home directory. If not found in the Home directory then copy
    it from /usr/share/doc using the following command:	
	cp -r /usr/share/doc/moose1.4 ~/. 

GUI version of MOOSE:
  - To start the GUI version of MOOSE is to start a terminal and type
    'moosegui.py'.
  - Once the GUI pops up, select a model (SBML or GENESIS SLI) to load,
    choose the objects you wish to plot, set the length of simulation and the
    plot interval. Finally hit 'Reset' and then 'Run'.

SHELL version of MOOSE:
  - To start the shell version of MOOSE is to start a terminal, and type
    'moose'.
  - To try out the Demos, you will need to start a terminal, change the directory
    to the location where you stored the demos:

	cd ~/moose1.4/<demo-path>

    Finally, run a script, let's say 'Squid.g' like this:

	moose Squid.g

About MOOSE:
MOOSE is the Multiscale Object-Oriented Simulation Environment. It spans the 
range from single molecules to subcellular networks, from single cells to 
neuronal networks, and to still larger systems. It is backwards-compatible with
GENESIS, and forward compatible with Python. A key goal of MOOSE is to
embrace many simulation and numerical engines that span a range of levels of 
simulation, and to provide the infrastructure to enable these to interoperate
seamlessly.

Numerical engines:
Crank-Nicolson solver for compartmental neuronal models and networks
Adaptive timestep Runge-Kutta solver for chemical kinetic systems
Gillespie Stochastic Systems Algorithm for chemical kinetics
Prototype Multiscale solver to interface electrical and signaling models.
Prototype Steady State solver to obtain steady states for chemical kinetics.
	Several of these solvers utilize the GNU Scientific Library.

Technical data:
Authors: Upinder S. Bhalla, Niraj Dudani, Subhasis Ray, Raamesh Deshpande
GUI work: Chaitanya, Harsha Rani
Programming and Release support: Siji George.
Web page: Harsha Rani.
All authors are at the National Centre for Biological Sciences, Bangalore, 
India.

Language: C++, Python.
Source available at SourceForge. Licence: LGPL. However, due to the
	terms of the GPL-licensed GSL, the effective license of the
	package as a whole is GPL.
Binaries available: Linux (32 and 64-bit), Solaris, Windows (32 bit), 
There is also a tarball for those who wish to compile

Please refer to the mirrored MOOSE websites for current information and
for a subversion code repository:
http://moose.ncbs.res.in
http://moose.sourceforge.net

Please look at the RELEASE_NOTES for updates and changes compared to the first
release. The earlier release notes are in the DOCS directory.
