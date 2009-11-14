Documentation for the MOOSE elements 'GLcell' and 'GLview' and the
graphical client 'glcellclient'
------------------------------------------------------------------


1. External libaries and compiling MOOSE with support for the GLcell
and GLview elements:
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

Specifically the following Boost libraries are required:

	     Serialization
	     Thread
	     Filesystem
	     System 

After these have been installed, MOOSE can be built with support for
GLcell and GLview by supplying the command-line parameter 'USE_OSG=1',
as in:

> make clean
> make USE_OSG=1

This is in the main MOOSE directory. An alternative is to export
USE_OSG as an environment variable with a value of 1:

> export USG_OSG=1
> make clean
> make




2. Using the GLcell element:
-----------------------

The GLcell element must be explicitly created and set up with
appropriate field values (some are optional) as described
below. GENESIS-style script examples are also provided:

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
-------------

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

f. 'sync': default value is 'off'. If the time taken by the client
instance to render one frame of color information (over all
compartments) exceeds the time taken by the MOOSE simulation to
simulate one time-step, the simulation may overtake the 3D display and
the 3D display will skip intermediate frames in order to keep up with
the state of the simulation.

This flag forces the two to operate in lockstep, which may be useful
for debugging or while recording movies for publication (so that no
frames are skipped) but will slow down the MOOSE simulation.

A longer explanation is provided in Appendix A of this document.

> setfield gl0 sync on
OR
> setfield gl0 sync off

g. 'vscale': default value is 1.0. Cylindrical compartments when drawn
in a physiologically accurate ratio between their diameters and
lengths can be too thin to render well visually. They can be scaled up
in thickness, only in terms of visual appearance and not numerically,
by specificying this value as greater than 1.0.

> setfield gl0 vscale 4

h. 'bgcolor': default value is 000000000. This represents the
background color of the viewer window, encoded as a string in the
format RRRGGGBBB, where 000000000 is black (the default) and 255255255
is white.

> setfield gl0 bgcolor 100100100

i. 'highvalue': default value is 0.05 (to be interpreted as 0.05V,
where the attribute is Vm). This is the value represented by the color
specified on the last line of the colormap file. This represents the
highest value that the attribute is assumed to be able to attain. Any
values greater than this, should they occur, will be mapped to the
color which corresponds to this value.

> setfield gl0 highvalue 0.1

j. 'lowvalue': default value is -0.1 (to be interpreted as -0.1V,
where the attribute is Vm). This is the value represented by the color
specified on the first line of the colormap file. This represents the
lowest value that the attribute is assumed to be able to attain. Any
values lower than this, should they occur, will be mapped to the color
which corresponds to this value.

> setfield gl0 lowvalue -0.08


A version of DEMOS/gbar/myelin.g, modified to include GLcell,
is included in the directory DEMOS/gbar/ as cMyelin.g. Part 4
of this document contains general operating instructions to launch
such a script, coupled with an instance of 'glcellclient', as a
complete example.

3. Using the GLview element:
-----------------------

The GLview element must be explicitly created and set up with
appropriate field values (some are optional) as described
below. GENESIS-style script examples are also provided:

> create GLview gl0
> setclock 4 0.01
> useclock gl0 4

The following information (indented) is adapted from the documentation
for 'xview' in GENESIS-2.3:

        This element displays values by linear interpolation. Every
	GLview has at least two child elements of class GLshape, named
	shape[0], shape[1], etc. These shapes determine the extreme
	points for the linear interpolation. Therefore if shape[0] has
	foreground color 0.0, and shape[1] has color 1.0, the range of
	colors displayed would be interpolated between 0.0 and
	1.0. Likewise, if shape[0] displays a tall thin rectangle, and
	shape[1] a short wide rectangle, the shapes displayed would be
	'morphed' between these two extremes (the current
	implementation of GLview supports cubes and spheres, so
	morphing refers to interpolation between cubes of different
	widths or spheres of different diameters). Of course, to get
	these effects one would have to set the color_val and
	morph_val to the appropriate values (see below for 'color_val'
	and 'morph_val').  In addition to the shapes themselves, three
	arrays are used to specify how the GLview element does
	interpolation. As already mentioned, the 'values' array
	specifies the actual values to be displayed (in GLview,
	'values' consists of five one-dimensional arrays, each
	specified by 'value1'..'value5').  'value_min' contains the
	values that correspond to the lower end of the interpolated
	range. Thus if the element at index 0 of the array represented
	by value1 was equal to value_min[0], the parameter displayed
	would correspond to shape[0]. Likewise, value_max contains the
	upper end of the interpolated range.

	One can have multiple GLshapes. In this case the interpolation
	first decides which pair of shapes to use. In this version,
	each pair of shapes handles an equal range between value_min
	and value_max.  Having selected the appropriate pair of
	shapes, the algorithm then proceeds as outlined above.

	By default, the GLview object creates two child shapes,
	shape[0] and shape[1]. The default shapes are cubes, shape[0]
	being a small one with color 0.0 and shape[1] a big one in
	color 1.0.  The child shapes can have parameters set in the
	usual ways, but are not displayed independently of the GLview
	element.

        The two child shapes are created when the GLview element
        receives its first RESET message, if they have not have been
        created by the user yet (manually). Therefore their fields
        (such as shapetype) can only be changed after at least one
        RESET has been sent to the GLview element in question.

	So, for instance:

        > create GLview gl0
        > reset
	This creates the two default child shapes.

        > setfield gl0/shape[0] shapetype 1
	This sets the shapetype of shape[0] to sphere, from the
	default of cube.

	Note that currently the shapetype of shape[0] is adopted as
	the shapetype of all 3D shapes that make up the visualization.

	GLshape elements have the following fields:

	'color': colors are specified from 0.0 to 1.0, and represent
	the full range of the colormap file referenced by the client.

	'xoffset', 'yoffset', 'zoffset': offsets in the x,y,z
	directions of the shape from its original location.

	'len': width if the shape is a cube, diameter if it is a sphere.

        'shapetype': 0 represents cubes (the default), and 1
        represents spheres. Please note that spheres consist of many
        more polygons than cubes and therefore displaying a large
        number of spheres can make the client slow and unresponsive.

Required fields:
-------------

a. 'path': this specifies the GENESIS-style path for elements whose
attributes we wish to visualize directly or after appending the value
of the attribute 'relpath' to each such element. The attribute 'path'
is usually specified as a wildcard.

The 3D co-ordinates of the shapes that make up the visualization are
obtained from elements on 'path' by the following heuristics,
whichever applies first:

1. If the particular element on 'path' itself has x, y, z fields, or
2. if the parent of the element has x, y, z fields or its parents and
so on, unless such a parent is the root element,

unless the x, y, z values thus obtained overlap those of another
shape. If such a collision occurs, or no x, y, z values could be
obtained in any case, the shape is automatically placed on a planar
grid among other such unplaced shapes. The planar grid is itself
located just outside the bounding box of shapes that were successfully
placed.

> readcell test.p /axon
> setfield gl0 path /axon/##[CLASS=Compartment]

b. 'port': this specifies the network port (on localhost or on a
remote machine) over which this GLcell element will communicate with
the corresponding client instance (the process 'glcellclient'):

> setfield gl0 port 9999

Optional fields:
-------------

d. 'relpath': default is blank. If this is assigned, it is appended to
the full path of each element found on 'path', and the element on the
resulting path is then checked for field values.

> setfield gl0 relpath soma

e. 'value1'..'value5': default values are blank. With respect to
elements found on 'path' (with 'relpath' appended to each element's
pathname, if non-blank), these attributes represent the names of
fields whose values will be used as attributes of 3D shapes in the
final visualization. Each such field ('value1'..'value5') thus
initializes an array of values in the GLview element, taken from the
named field per element on 'path'.

> setfield gl0 value1 Vm

The following five fields specify which array of values is used to
modify which attribute of the 3D shapes that appear in the
visualization. Each has the default value of '0', and since there is
no 'value0' or corresponding array, the final shapes are not modified
by default (they appear as grey cubes of size 0.5 in this case).

 f. 'color_val': this specifies the index of the array that modifies
 the color of the 3D shapes in the visualization as per the client's
 current colormap.

> setfield gl0 color_val 1

So, for instance, the above line will make each shape's color be
mapped to the field represented by 'value1' in elements on 'path'
(with 'relpath' appended to each element's pathname if present). It is
assumed that 'value1' has already been set and values extracted
successfully.

g. 'morph_val': this specifies the index of the array that modifies
the size of the 3D shapes (width or diameter, depending on type of
shape) in the visualization.

> setfield gl0 morph_val 1

h. 'xoffset_val'

i. 'yoffset_val'

j. 'zoffset_val'

These specify the indices of the arrays, respectively, that modify the
offset in the x, y, z directions of 3D shapes from their original
locations.

k. 'set_valuemin' and 'set_valuemax'

This sets value_min and value_max values for the specified array.
This accepts two arguments, the first of which is the index of the
array (1..5) and the second is the value to be set.

> call gl0 set_valuemin 1 -0.1
> call gl0 set_valuemax 1 0.05

So, for instance, this sets value_min for 'value1' to -0.1 and
value_max for 'value1' to 0.05.

l. 'host': default value is '127.0.0.1'. This specifies the machine on
which the corresponding client instance is running.

> setfield gl0 host localhost

m. 'sync': default value is 'off'. If the time taken by the client
instance to render one frame of color information (over all
compartments) exceeds the time taken by the MOOSE simulation to
simulate one time-step, the simulation may overtake the 3D display and
the 3D display will skip intermediate frames in order to keep up with
the state of the simulation.

This flag forces the two to operate in lockstep, which may be useful
for debugging or while recording movies for publication (so that no
frames are skipped) but will slow down the MOOSE simulation.

A longer explanation is provided in Appendix A of GLcell.txt.

> setfield gl0 sync on
OR
> setfield gl0 sync off

n. 'bgcolor': default value is 000000000. This represents the
background color of the viewer window, encoded as a string in the
format RRRGGGBBB, where 000000000 is black (the default) and 255255255
is white.

> setfield gl0 bgcolor 100100100



A version of DEMOS/gbar/myelin.g, modified to include GLview, is
included in the directory DEMOS/gbar/ as vMyelin.g. Part 4 of this
document contains general operating instructions to launch such a
script, coupled with an instance of 'glcellclient', as a complete
example.

4. Using 'glcellclient':
-----------------

Command-line parameters:
----------------------

Required:
--------

-p port-number: where port-number is the numerically specified network
 port over which the corresponding GLcell element will talk to this
 instance. This should be the same as the 'port' field as specified in
 the GLcell element.

-c colormap-file: where colormap-file is the name of a file specifying
 colors to which each compartment will be mapped as per its value with
 respect to the attribute under observation. These files carry over
 from the GENESIS environment and four are included in the GLcell
 source directory, viz. 'rainbow2','redhot','grey' and 'hot'.

-m mode: where mode specifies whether the current instance of
glcellclient is to connect to an instance of GLcell or one of
GLview. Possible values are 'c' and 'v'; 'c' is for use with GLcell
and 'v' is for use with GLview.


Optional:
--------

-d directory: with a default value of './', the current
 directory. This is the full pathname of the path to which any
 screenshots, single or sequential, will be saved.

-a angle: with a default value of 10 degrees, where angle is the
angular increment in the drawing of the sides of curved
bodies. Smaller numbers give bodies that are smoother in appearance
but more expensive to render. This value is required to be between 1
and 60 degrees. Any value not between 1 and 60 will be capped to the
closer limit.


Keyboard commands accepted in the viewer window include:

's': cycles through four states of graphics statistics overlaid on the
window and one state of none.

'c' or 'C': captures and saves single screenshot to the path
specified.

'm' or 'M': toggles continuous saving of sequential images, one per
frame, to the path specified. These can be stitched into a movie file
with third-party software such as 'ffmpeg'. Note that this mode can
consume disk-space at a high rate.



Keyboard and mouse bindings:
-------------------------

SPACE key:				Reset camera position and
      					        model position and zoom

LEFT mouse button: 		Rotate camera around model

MIDDLE mouse button or
LEFT and RIGHT mouse
buttons together:			Pan model

RIGHT mouse button or		Zoom model or truck-in camera
mousewheel up/down:  	      	(if minimum zoom reached)

p or P key:				Switch between
						perspective (default) and
						orthographic views

c or C key:				Capture single frame to
       						image file

m or M key:				Toggle sequential auto-capture
       					        of frames to image files

5. Operating instructions:
---------------------

The package is launched in two steps; this can be simplified to a
single step with a PyMOOSE wrapper built around the simulation.

a. Launching the 'glcellclient' client instance:

>./glcellclient -p 9999 -c ../colormaps/rainbow2 -m c 
(for use with GLcell) OR
> ./glcellclient -p 9999 -c ../colormaps/rainbow2 -m v
(for use with GLview)

b. Starting a MOOSE simulation from a script which includes the
creation and initialization of the GLcell/GLview element: 

An example script for GLcell is included in the directory DEMOS/gbar/
in the MOOSE distribution, and is named cMyelin.g.

>cd DEMOS/gbar
>../../moose cMyelin.g

A version of this example script modified to demonstrate GLview is
included in the same directory and named vMyelin.g.

>cd DEMOS/gbar
>../../moose vMyelin.g



Appendix A (explanation of network flow between GLcell|GLview and
glcellclient) :
-------------------------------------------------------

(Please note that the attached images mention only GLcell but apply
equally to GLview, which was developed later)

GLcell and GLview are single-threaded whereas glcellclient runs in two
separate threads. The first thread is responsible for network
communication with GLcell|GLview and the second to render graphics,
process incoming color updates from GLcell|GLview and handle picking
events invoked by the user which are transmitted back to GLcell|GLview
via the first thread.

An overview of the flow of network communication in the two modes
(given by the two states of the 'sync' flag) is provided in the two
figures accompanying this document:

a. When 'sync' is 'on':

Please refer to the sequence diagram in fig_sync_on.png.

  In this mode, GLcell|GLview will await an acknowledgement message
from the client after every transmission of updated color data to it,
before any new color data (if updated) is transmitted on the next
clock pulse. The client instance in turn will only send this
acknowledgement after the latest colors have been updated in the
display. This ensures that the client instance displays every color
change sent to it during the simulation. This is intended to be useful
while recording sequential images to be stitched into a movie of the
simulation.

b. When 'sync' is 'off':

Please refer to the sequence diagram in fig_sync_off.png.

  In this mode, the client instance will always display the last set
of colors transmitted to it by GLcell|GLview. Intermediate frames may
be skipped. The simulation in this mode will proceed faster than when
'sync' is 'on'.
