//moose

create KineticManager /m
createmap Molecule /m 1 2 -object
setfield /m/Molecule[0] nInit 1
setfield /m/Molecule[1] nInit 0
createmap Reaction /m 1 2 -object
addmsg /m/Molecule[0] /m/Reaction[0] SUBSTRATE n
addmsg /m/Reaction[0] /m/Molecule[0] REAC A B

addmsg /m/Molecule[1] /m/Reaction[0] PRODUCT n
addmsg /m/Reaction[0] /m/Molecule[1] REAC B A

setfield /m method rk5
reset
showfield /m/Molecule[] n
step 10 -t
showfield /m/Molecule[] n
