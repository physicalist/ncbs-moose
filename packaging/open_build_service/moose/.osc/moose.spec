%global branch 3.0.0
%define _unpackaged_files_terminate_build 0 
Name: moose
Summary: MOOSE is the Multiscale Object-Oriented Simulation Environment
Version: 3.0.0
Release: 1%{?dist}
Url: http://sourceforge.net/projects/moose
Source0: moose-%{branch}.tar.bz2

License: LGPLv2

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: python-devel
BuildRequires: gsl-devel
BuildRequires: hdf5-devel
BuildRequires: numpy
BuildRequires: python-setuptools
BuildRequires: libxml2-devel

%description
MOOSE is the base and numerical core for large, detailed simulations
including Computational Neuroscience and Systems Biology. MOOSE spans
the range from single molecules to subcellular networks, from single
cells to neuronal networks, and to still larger systems. It is
backwards-compatible with GENESIS, and forward compatible with Python
and XML-based model definition standards like SBML and NeuroML.

MOOSE uses Python as its primary scripting language. For backward
compatibility we have a GENESIS scripting module, but this is
deprecated. MOOSE uses Qt/OpenGL for its graphical interface. The
entire GUI is written in Python, and the MOOSE numerical code is
written in C++.

%package all
Summary: Meta package of MOOSE
%description -n %{name}-all
This is the meta package of MOOSE. It installs all components of MOOSE
simulator.

Requires: %{name}-gui
Requires: %{name}-python

%package core
Summary: Core package of MOOSE simulator
%description -n %{name}-core
This package contains C++ core of MOOSE simulator. It is intended for clusters.
For general purpose MOOSE with python scripting support, install moose-python.

Requires: gsl
Requires: hdf5

%package python
Summary: Python-2 interface for %{name}
%description python
This package contains %{_summary}.

Requires: numpy
Requires: PyQt4
Requires: gsl
Requires: PyOpenGL
Requires: python-matplotlib-tk
Requires: libxml2


%package gui
Summary: GUI frontend of MOOSE simulator
%description gui
GUI frontend of MOOSE. It uses openscenegraph to visualize neural networks.
Requires: OpenSceneGraph
Requires: moose-python

%prep
%setup -q -n %{name}-%{branch}

%build
cd moose_3.0.0
mkdir -p build
cd build && cmake .. && make %{?_smp_mflags}

%install
cd moose_3.0.0
( cd build && make install DESTDIR=$RPM_BUILD_ROOT )
( cd python && python2 setup.py install --root $RPM_BUILD_ROOT )

%files core 
%defattr(-,root,root)
/usr/bin/moose
/usr/lib/libmoose.so

%files python
%defattr(-,root,root)
%{python_sitelib}/*

%files gui
%defattr(-,root,root)
/usr/bin/moosegui
/usr/share/moose
%dir /usr/share/moose/gui
