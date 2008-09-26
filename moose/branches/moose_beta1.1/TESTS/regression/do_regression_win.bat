@echo off

set MOOSE=..\..\moose_vcpp2005\release\moose

REM nearDiff is a function to see if two data files are within epsilon of each other. It assumes the files are in xplot format.If it succeeds, it prints out a period without a newline.If it fails, it prints out the first argument and indicates where it failed.

set NEARDIFF=.\neardiff

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_squid.g > NUL
%NEARDIFF% moose_squid.plot test.plot 5.0e-6
ECHO squid
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_kholodenko.g > NUL
%NEARDIFF% moose_kholodenko.plot test.plot 1.0e-5
ECHO kinetics
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_readcell_global_parms.g >NUL 
%NEARDIFF% moose_readcell_global_parms.plot test.plot 1.0e-5 -fractional
ECHO readcell1
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_readcell.g >NUL 
%NEARDIFF% moose_readcell.plot test.plot 5.0e-3
ECHO readcell2
TYPE NUL


IF EXIST test.plot ERASE test.plot
%MOOSE% moose_synchan.g > NUL
%NEARDIFF% moose_synchan.plot test.plot 5.0e-11
ECHO "synchan"
TYPE NUL

REM There is a small numerical divergence between moose and genesis on the upswing of the synchan. This is because of how they handle the timing event arrival, so the MOOSE version is one dt later.

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_channels.g > NUL
FOR %%i IN ( Ca_hip_traub91 K2_mit_usb KA_bsg_yka Ka_hip_traub91 Kahp_hip_traub91 K_bsg_yka Kdr_hip_traub91 K_hh_tchan KM_bsg_yka K_mit_usb LCa3_mit_usb Na_bsg_yka Na_hh_tchan Na_hip_traub91 Na_mit_usb Na_rat_smsnn ) DO %NEARDIFF% channelplots\moose_%%i.plot test_%%i.plot 5.0e-2 -f
ECHO "channels"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_network.g > NUL
%NEARDIFF% moose_network.plot test.plot 1.0e-3
ECHO "network"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_axon.g > NUL
%NEARDIFF% moose_axon.plot test.plot 5.0e-3
ECHO "axon"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_mitral.g > NUL
%NEARDIFF% moose_mitral.plot test.plot 5.0e-3
ECHO "mitral"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_rallpack1.g > NUL
%NEARDIFF% moose_rallpack1.plot test.plot 5.0e-5
ECHO "rallpack1"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_rallpack2.g > NUL
%NEARDIFF% moose_rallpack2.plot test.plot 5.0e-6
ECHO "rallpack2"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_rallpack3.g > NUL
%NEARDIFF% moose_rallpack3.plot test.plot 5.0e-2
ECHO "rallpack3"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_synapse.g> NUL
%NEARDIFF% moose_synapse.plot test.plot 5.0e-7
ECHO "synapse"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_traub91.g > NUL
%NEARDIFF% moose_traub91.plot test.plot 5.0e-2
ECHO "traub91"
TYPE NUL

IF EXIST test.plot ERASE test.plot
%MOOSE% moose_file2tab2file.g > NUL
%NEARDIFF% moose_file2tab.plot test.plot 1.0e-6
ECHO "file2tab and tab2file"
TYPE NUL

IF EXIST test.plot ERASE test.plot
ERASE test_*.plot
