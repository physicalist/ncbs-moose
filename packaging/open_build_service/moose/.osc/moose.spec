%global branch 3.0.1
%define _unpackaged_files_terminate_build 0 
Name: moose
Summary: MOOSE is the Multiscale Object-Oriented Simulation Environment
Version: 3.0.1
Release: 1%{?dist}
Url: http://sourceforge.net/projects/moose
Source0: moose-%{branch}.tar.bz2

License: GPL-3.0

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: python-devel
BuildRequires: gsl-devel
BuildRequires: hdf5-devel
BuildRequires: numpy
BuildRequires: python-setuptools
BuildRequires: libxml2-devel
BuildRequires: libbz2-devel

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

%package -n %{name}-all
Summary: Meta package of MOOSE
%description -n %{name}-all
This is the meta package of MOOSE. It installs all components of MOOSE
simulator.

Requires: %{name}-gui
Requires: %{name}-python

%package -n libmoose-3
Summary: Library of MOOSE simulator
%description -n libmoose-3
This package contains C++ core of MOOSE simulator. It is intended for clusters.
For general purpose MOOSE with python scripting support, install moose-python.

Requires: gsl
Requires: hdf5
Requires: bzip2
Requires: libxml2
Requires: bzip2
Requires: libxml2

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
Requires: bzip2
Requires: hdf5
Requires: python-networkx


%package gui
Summary: GUI frontend
%description gui
GUI frontend. It uses openscenegraph to visualize neural networks.
Requires: OpenSceneGraph
Requires: moose-python

%prep
%setup -q -n %{name}-%{branch}

%build
mkdir -p _build
cd _build && cmake .. && make 

%install
cd _build && make install DESTDIR=$RPM_BUILD_ROOT

%files -n libmoose-3
%defattr(-,root,root)
%{_bindir}/moose
%{_prefix}/lib/libmoose.so.3

%post -n libmoose-3 -p /sbin/ldconfig

%files python
%defattr(-,root,root)
%dir %{_prefix}/lib/moose/python

%post python
cd %{_prefix}/lib/moose/python && %{__python2} setup.py install


%files gui
%defattr(-,root,root)
%{_bindir}/moosegui
%{_prefix}/lib/moose
%dir %{_prefix}/lib/moose/gui
