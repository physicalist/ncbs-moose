Documentation for the MOOSE element 'GLcell' and the graphical client
'glcellclient'
------------------------------------------------------------------


1. External libaries and compiling MOOSE with support for the GLcell
element:
--------------------------------------------------------------

This package has been built with the following external
libaries. These must be compiled and installed on the target machine
before this package can be expected to be functional. Although
previous versions of each external library as indicated may work, it
is recommended that the versions listed (or newer) are installed for
the use of this package.

a. OpenSceneGraph, Stable Release 2.8.0: 
http://www.openscenegraph.org/projects/osg/wiki/Downloads

b. Boost C++ libaries, Version 1.39.0:
http://www.boost.org/users/download/

Specifically the following libraries are needed, although installing
the entire suite is recommended:

	     Serialization
	     Thread
	     Filesystem
	     System 


After these have been installed, MOOSE can be built with support for
GLcell by supplying the command-line parameter 'USE_OSG=1', as in:

> make clean
> make USE_OSG=1

This is in the main MOOSE directory. An alternative is to export
USE_OSG as an environment variable with a value of 1:

> export USG_OSG=1
> make clean
> make




2. Using the GLcell element:
----------------------

The GLcell element must be created, connected to a clock and field
values supplied (some of which are optional). GENESIS-style script
examples follow:

> create GLcell gl0
> setclock 4 0.01
> useclock gl0 4

Required fields:
-------------

a. 'path': this specifies the GENESIS-style path for the geometry to
be visualized; this will have been loaded previously with the
'readcell' script command:

> readcell test.p /axon
> setfield gl0 path /axon

b. 'port': this specifies the network port (on localhost or on a
remote machine) over which this GLcell element will communicate with
the corresponding client instance (the process 'glcellclient'):

> setfield gl0 port 9999


Optional fields:
------------

c. 'host': default value is '127.0.0.1'. This specifies the machine on
which the corresponding client instance is running.

> setfield gl0 host localhost

d. 'attribute': default value is 'Vm'. This is the field under
observation. Values for this field will be mapped to color ranges as
specified by a colormap file passed to the client instance.

>setfield gl0 attribute Vm

e. 'threshold': default value is 1e-8. Changes in the 'attribute'
field's value smaller than the value of 'threshold' are not
transmitted to the client instance. Values are compared against the
last value transmitted for the specific compartment, not necessarily
the value at the last clock pulse. The intent is to reduce the
bandwidth of transmission between the GLcell element and the client
instance; this substantially improves the performance of the client
instance.

> setfield gl0 threshold 0.0015

f. 'sync': default value is 'off'. If 'sync' is 'on', the GLcell
element will await an acknowledgement message from the client after
every data transmission to it, before it can return and transmit new
data on the next clock pulse. This ensures that the client instance
can display every frame during the simulation, and can be useful while
recording sequential images to be stitched into a movie of the
simulation. If 'sync' is 'off', the client instance will always
display the latest frame; it may skip intermediate frames so that the
simulation will proceed faster than when 'sync' is 'on'.

> setfield gl0 sync on


A version of DEMOS/gbar/myelin.g, modified to include GLcell,
is included in the directory DEMOS/gbar/ as cMyelin.g. Part 4
of this document contains general operating instructions to launch
such a script, coupled with an instance of 'glcellclient', as a
complete example.





3. Using 'glcellclient':
-----------------

The executable 'glcellclient' can be launched with the following
command-line parameters, the first of two of which are required:

-p port-number: where port-number is the numerically specified network
 port over which the corresponding GLcell element will talk to this
 instance. This should be the same as the 'port' field as specified in
 the GLcell element.

-c colormap-file: where colormap-file is the name of a file specifying
 colors to which each compartment will be mapped as per its value with
 respect to the attribute under observation. These files carry over
 from the GENESIS environment and four are included in the GLcell
 source directory, viz. 'rainbow2','redhot','grey' and 'hot'.


The remaining command-line parameters are optional:

-u upper: with a default value of 0.05V, where upper is the value
 represented by the color specified on the last line of the colormap
 file. This represents the highest value that the attribute is assumed
 to be able to attain. Any values greater than this, should they
 occur, will be mapped to the same color which corresponds to this
 value.

-l lower: with a default value of -0.1V, where lower is the value
 represented by the color specified on the first line of the colormap
 file. This represents the lowest value that the attribute is assumed
 to be able to attain. Any values lower than this, should they occur,
 will be mapped to the color which corresponds to this value.

-d directory: with a default value of './', the current
 directory. This is the full pathname of the path to which any
 screenshots, single or sequential, will be saved.


Keyboard commands accepted in the viewer window include:

's': cycles through four states of graphics statistics overlaid on the
window and one state of none.

'c' or 'C': captures and saves single screenshot to the path
specified.

'm' or 'M': toggles continuous saving of sequential images, one per
frame, to the path specified. These can be stitched into a movie file
with third-party software such as 'ffmpeg'. Note that this mode can
consume disk-space at a high rate.






4. Operating instructions:
---------------------

The package is launched in two steps; this can be simplified to a
single step with a PyMOOSE wrapper built around the simulation.

a. Launching the 'glcellclient' client instance:

>./glcellclient -p 9999 -c rainbow2 


b. Starting a MOOSE simulation from a script which includes the
creation and initialization of a GLcell element: such a script is
included in the directory DEMOS/gbar/ in the MOOSE distribution,
and is named cMyelin.g.

>cd DEMOS/gbar
>../../moose cMyelin.g
