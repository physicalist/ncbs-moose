//moose


/* Calling newly added command for finding the start execution time of the scipt - Aastha - Start 220807 */
starttime
/* Calling newly added command for finding the start execution time of the scipt - Aastha - End 220807 */



create compartment /c1
create compartment /c2

addmsg /c1 /c2 AXIAL Vm
addmsg /c2 /c1 RAXIAL Ra Vm

showmsg /c1

showmsg /c2



/* Calling newly added command for finding the end  time of the scipt - Aastha - Start 220807 */
endtime
/* Calling newly added command for finding the end  time of the scipt - Aastha - End 220807 */



