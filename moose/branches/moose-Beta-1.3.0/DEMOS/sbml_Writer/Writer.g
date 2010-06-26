include plotUtil.g

str infile = "end_prd_inhib.xml"    //file to write
str outfile = "moose.plot" //simulation result
int plot_all = 1
str plot_some = " "   

str target = "/kinetics"

int USE_SOLVER = 1 
float SIMLENGTH = 500000  //simulation time
float SIMDT = 1000
float IODT = 1.0
int IOCLOCK = 2

setclock 0 {SIMDT}
setclock 1 {SIMDT}
setclock 2 {IODT}

include end_prd_inhib.g //genesis file is read into moose
writeSBML { infile } { target }  //write xml file

init_plots { SIMLENGTH } { IOCLOCK } { IODT }

str mol
if ( plot_all )
	foreach mol ( { el { target }/##[TYPE=kpool] } )
		echo "Adding plot: "{ mol }
		add_plot { mol } Co { outfile }
	end
else
	foreach mol ( { arglist { plot_some } } )
		echo "Adding plot: "{ mol }
		add_plot { mol } Co { outfile }
	end
end

if ( ! USE_SOLVER )
	setfield { target } method "ee"
end
reset
step {SIMLENGTH} -time

/* Clear the output file */
openfile { outfile } w
closefile { outfile }

save_plots
echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Plots written to *.plot. 

If you have gnuplot, run 'gnuplot plot.gnuplot' to view the graphs.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"
quit

