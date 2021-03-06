#!/bin/tcsh

set MOOSE = ../../moose

# nearDiff is a function to see if two data files are within 
# epsilon of each other. It assumes the files are in xplot format
# If it succeeds, it prints out a period without a newline.
# If it fails, it prints out the first argument and indicates where it
# failed
set NEARDIFF = ./neardiff

/bin/rm -f test.plot
$MOOSE moose_squid.g > /dev/null
echo -n squid
$NEARDIFF moose_squid.plot test.plot 1.0e-5

/bin/rm -f test.plot
$MOOSE moose_kholodenko.g > /dev/null
echo -n kinetics
$NEARDIFF moose_kholodenko.plot test.plot 1.0e-5

/bin/rm -f test.plot
$MOOSE moose_readcell_global_parms.g > /dev/null
echo -n readcell1
$NEARDIFF moose_readcell_global_parms.plot test.plot 1.0e-5 -fractional

/bin/rm -f test.plot
$MOOSE moose_single_compt.g > /dev/null
echo -n single_compt
$NEARDIFF moose_single_compt.plot test.plot 1.0e-3

/bin/rm -f test.plot
$MOOSE moose_readcell.g > /dev/null
echo -n readcell2
$NEARDIFF moose_readcell.plot test.plot 5.0e-3

/bin/rm -f test.plot
$MOOSE moose_synchan.g > /dev/null
echo -n "synchan"
$NEARDIFF moose_synchan.plot test.plot 5.0e-11
# There is a small numerical divergence between moose and genesis
# on the upswing of the synchan. This is because of how they handle the
# timing event arrival, so the MOOSE version is one dt later.

/bin/rm -f test.plot
echo -n "channels"
$MOOSE moose_channels.g > /dev/null

######################################################################
# These are all the channels.
# Bad channels are 
# Ca_bsg_yka, Ca_hip_traub, Kca_hip_traub, Kc_hip_traub91, K_hip_traub
# K_bsg_yka is a little bit off.
#
#foreach i ( Ca_bsg_yka Ca_hip_traub91 Ca_hip_traub K2_mit_usb KA_bsg_yka Ka_hip_traub91 Kahp_hip_traub91 K_bsg_yka Kca_hip_traub Kc_hip_traub91 Kdr_hip_traub91 K_hh_tchan K_hip_traub KM_bsg_yka K_mit_usb LCa3_mit_usb Na_bsg_yka Na_hh_tchan Na_hip_traub91 Na_mit_usb Na_rat_smsnn )
######################################################################

foreach i ( Ca_hip_traub91 K2_mit_usb KA_bsg_yka Ka_hip_traub91 Kahp_hip_traub91 K_bsg_yka Kdr_hip_traub91 K_hh_tchan KM_bsg_yka K_mit_usb LCa3_mit_usb Na_bsg_yka Na_hh_tchan Na_hip_traub91 Na_mit_usb Na_rat_smsnn )
	echo -n $i
	$NEARDIFF channelplots/moose_$i.plot test_$i.plot 5.0e-2 -f
end

/bin/rm -f test.plot
$MOOSE moose_synapse_solve.g > /dev/null
echo -n "solver|readcell|synchan"
$NEARDIFF moose_synapse_solve.plot test.plot 1.0e-3

/bin/rm -f test.plot
$MOOSE moose_network.g > /dev/null
echo -n "network"
$NEARDIFF moose_network.plot test.plot 1.0e-11

/bin/rm -f test.plot
$MOOSE moose_file2tab2file.g > /dev/null
echo -n "file2tab and tab2file"
$NEARDIFF moose_file2tab.plot test.plot 1.0e-6

echo
/bin/rm -f test.plot
/bin/rm -f test_*.plot
