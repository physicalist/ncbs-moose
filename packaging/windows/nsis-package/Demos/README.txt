This directory has demo simulations for MOOSE. It is possible that you
may not have write access to the demo directories (in case they are
located in C:\Program Files\MOOSE\Demos). In such a case copy the Demos
directory to your home folder, and execute the scripts from there. This
is required since many of the example scripts generate file output, and
need write access.

There are 2 ways to run the simulation scripts:

1) The simplest way is to just double-click on the script's file icon.
   If there are multiple .g files in a directory, then the one you
   should run is the one beginning with a Capital letter.

2) The other way is to launch moose explicitly from a terminal. To open
   a terminal window, goto Start > Run, and type 'cmd' (without quotes)
   and then click on OK. If the directory containing your script is not
   in C: and is in, say, D: then you need to switch the drive using the
   command:
   
        d:
   
   or the appropriate drive letter in place of 'd'. Then you change the
   current directory to the one containing your simulation script,
   using the command:
   
        cd <full-directory-path>
   
   and hit Enter.
   
   Finally, you run the script by giving the command:
        moose <script-name>
   
   If the script's file name is very long, or has spaces in it, then
   MOOSE may not be able to handle it. In that case, it is best to use
   the first method (double-clicking) given above, or renaming the
   script file.


Directory	Command		Output file(s)		NOTES
axon		moose Axon.g	axon*.moose.plot	Another axon model.
channels	moose Channels.g  *.moose.plot		Tests channel properties
kholodenko	moose Kholodenko.g test.plot		Kinetic simuation test
							Output not compatible
							with gnuplot.
							Ignore warnings about 
							simundumpfunc and setfield
mitral-ee	moose Mitral.g	mitral.moose.plot	Large mitral cell model
myelin		moose Myelin.g	*.moose.plot		Myelinated axon
network-ee	moose Network.g				Models a small network
pymoose		python squid.py				Needs moose.py
rallpack1	moose Rallpack1.g cable-*.moose.plot	Passive cable test
rallpack2	moose Rallpack2.g branch*.moose.plot	Branched passive cable
rallpack3	moose Rallpack3.g axon_*.moose.plot	Hodkin-Huxley axon 
squid-ee	moose Squid.g	squid.moose.plot	Hodkin-Huxley Squid model
synapse		moose Synapse.g	*.moose.plot		Synaptic transmission
traub91		moose Traub91.g	*.moose.plot		Traub CA3 cell model

In most subdirectories you can then use gnuplot to look at the output using
the command:

gnuplot plot.gnuplot
