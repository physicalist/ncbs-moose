Summary: Multiscale Object-Oriented Simulation Environment
Name: moose
Version: 1.3
Release: 0
License: LGPL
Group: Applications/Science
URL: http://moose.ncbs.res.in/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: libstdc++ >= 4.1.2, readline >= 5
BuildRequires: libstdc++ >= 4.1.2, readline >= 5, readline-devel >= 5, gsl >= 1.11, gsl-devel >= 1.11, python26-devel >= 2.6, swig >= 1.3.29
%description
MOOSE is the Multiscale Object-Oriented Simulation Environment. 
 It is the base and numerical core for large, detailed simulations including 
 Computational Neuroscience and Systems Biology. 
 . 
 MOOSE spans the range from single molecules to subcellular networks, from 
 single cells to neuronal networks, and to still larger systems. It is 
 backwards-compatible with GENESIS, and forward compatible with Python 
 and XML-based model definition standards like SBML and MorphML. It supports
 transparent parallelization, that is, it can run models on a cluster with no
 user effort required to parallelize them.
%prep
%setup -q

%build
make pymoose USE_GL=0 USE_NEUROML=1
mv external/neuroML_src/libneuroml.so ./
mv _moose.so _moose.so.tmp
mv moose.py moose.py.tmp
make clean USE_NEUROML=1
make moose USE_GL=0 USE_NEUROML=1
mv external/neuroML_src/libneuroml.a ./
g++ TESTS/regression/neardiff.cpp -o TESTS/regression/neardiff
%install
rm -rf $RPM_BUILD_ROOT
mkdir -p "$RPM_BUILD_ROOT/usr/bin"
mkdir -p "$RPM_BUILD_ROOT/usr/share/doc/moose1.3"
mkdir -p "$RPM_BUILD_ROOT/usr/share/man/man1"
mkdir -p "$RPM_BUILD_ROOT/usr/share/info"
mkdir -p "$RPM_BUILD_ROOT/usr/share/moose1.3/lib"
mkdir -p "$RPM_BUILD_ROOT/usr/share/moose1.3/py_stage"
install -m 755 moosegui "$RPM_BUILD_ROOT/usr/bin/"
install -s -m 755 moose "$RPM_BUILD_ROOT/usr/bin/moose-bin"
install -s -m 644 _moose.so.tmp "$RPM_BUILD_ROOT/usr/share/moose1.3/py_stage/_moose.so"
install -m 755 moose.py.tmp "$RPM_BUILD_ROOT/usr/share/moose1.3/py_stage/moose.py"
install -m 755 pymoose/pymoose.py "$RPM_BUILD_ROOT/usr/share/moose1.3/py_stage/"
cp -r DEMOS "$RPM_BUILD_ROOT/usr/share/doc/moose1.3/"
cp -r TESTS "$RPM_BUILD_ROOT/usr/share/doc/moose1.3/"
cp -r DOCS "$RPM_BUILD_ROOT/usr/share/doc/moose1.3/"
cp -r pymoose/gui/qt "$RPM_BUILD_ROOT/usr/share/moose1.3/moosegui"
cp -r gl/colormaps "$RPM_BUILD_ROOT/usr/share/moose1.3/colormaps/"
install -m 644 "DOCS/Beta-1.3/moose.1" "$RPM_BUILD_ROOT/usr/share/man/man1/"
install -m 644 DOCS/pymoose/pymoose.info "$RPM_BUILD_ROOT/usr/share/info/pymoose.info"
install -s -m 755 TESTS/regression/neardiff  "$RPM_BUILD_ROOT/usr/share/doc/moose1.3/TESTS/regression/"
install -m 755 TESTS/regression/do_regression.bat "$RPM_BUILD_ROOT/usr/share/doc/moose1.3/TESTS/regression/"
chmod 755 "$RPM_BUILD_ROOT/usr/share/moose1.3/moosegui/moosegui.py"

# Copy the libraries to lib directory in moose-shared deirectory
#( ldd _moose.so; ldd gl/src/glclient )    | while read line; do 
( ldd _moose.so.tmp)    | while read line; do 
    a=`echo $line |sed -e 's/(.*$//'`; 
    b=`echo $a | cut -s -d '>' -f2`
    if [ ! -z `echo $b | tr -d [:space:]` ]; then
	for libname in libsbml libxml2 libgsl libosg libOpenThreads libboost; do
	    echo $b $libname 
	    if [[ $b =~ $libname ]] ; then
		echo "Copying $b to lib/"  
		install -m 644 $b "$RPM_BUILD_ROOT/usr/share/moose1.3/lib"
		break
	     fi
	done
    fi
done

%post
ln -s "$RPM_BUILD_ROOT/usr/share/moose1.3/moosegui/moosegui.py" "$RPM_BUILD_ROOT/usr/bin/moosegui-bin"
echo "$RPM_BUILD_ROOT/usr/share/moose1.3/lib/" > /etc/ld.so.conf.d/moose.conf
ldconfig
install-info --infodir="$RPM_BUILD_ROOT/usr/share/info" "$RPM_BUILD_ROOT/usr/share/info/pymoose.info"

%clean
rm -rf $RPM_BUILD_ROOT

%preun
if [ -d /usr/share/moose1.3 ];
then
	rm -r /usr/share/moose1.3
fi
if [ -d /usr/share/doc/moose1.3 ];
then
	rm -r /usr/share/doc/moose1.3
fi
rm /usr/bin/moosegui-bin
if [ -f /usr/lib/python2.6/dist-packages/_moose.so ];
then 
	rm /usr/lib/python2.6/dist-packages/_moose.so
fi
if [ -f /usr/lib/python2.6/dist-packages/moose.py ];
then 
	rm /usr/lib/python2.6/dist-packages/moose.py
fi
if [ -f /usr/lib/python2.6/dist-packages/moose.pyc ];
then
	rm /usr/lib/python2.6/dist-packages/moose.pyc
fi
if [ -f /usr/lib/python2.5/site-packages/_moose.so ];
then 
	rm /usr/lib/python2.5/site-packages/_moose.so
fi
if [ -f /usr/lib/python2.5/site-packages/moose.py ];
then 
	rm /usr/lib/python2.5/site-packages/moose.py
fi
if [ -f /usr/lib/python2.5/site-packages/moose.pyc ];
then
	rm /usr/lib/python2.5/site-packages/moose.pyc
fi
if [ -f /etc/ld.so.conf.d/moose.conf ];
then
	rm /etc/ld.so.conf.d/moose.conf
fi
if [ -f /usr/share/info/pymoose.info ];
then
	sudo install-info --remove --info-dir=/usr/share/info /usr/share/info/pymoose.info
fi

%postun
ldconfig

%files
%defattr(-,root,root,-)
%doc README INSTALL COPYING.LIB /usr/share/man/man1/moose.1 /usr/share/info/pymoose.info


%changelog
* Thu Nov 11 2010  <subhasis@ncbs.res.in> - beta-1.3.0
- Initial RPM build.

