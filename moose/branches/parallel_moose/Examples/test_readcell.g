//moose

/* Calling newly added command for finding the start execution time of the scipt - Aastha - Start 220807 */
starttime
/* Calling newly added command for finding the start execution time of the scipt - Aastha - End 220807 */



include bulbchan.g

create neutral /library

ce /library
make_LCa3_mit_usb
make_Na_rat_smsnn
make_KA_bsg_yka
make_KM_bsg_yka
make_K_mit_usb
make_K2_mit_usb
make_Na_mit_usb
// make_Kca_mit_usb
make_Ca_mit_conc
ce /

readcell mit.p /mit


/* Calling newly added command for finding the end  time of the scipt - Aastha - Start 220807 */
endtime
/* Calling newly added command for finding the end  time of the scipt - Aastha - End 220807 */



