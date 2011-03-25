-*- mode: compilation; default-directory: "~/src/pymoose2/" -*-
Compilation started at Fri Mar 25 16:51:24

make pymoose BUILD=debug
make[1]: Entering directory `/home/subha/src/pymoose2/basecode'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/basecode'
make[1]: Entering directory `/home/subha/src/pymoose2/msg'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/msg'
make[1]: Entering directory `/home/subha/src/pymoose2/shell'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/shell'
make[1]: Entering directory `/home/subha/src/pymoose2/biophysics'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/biophysics'
make[1]: Entering directory `/home/subha/src/pymoose2/randnum'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/randnum'
make[1]: Entering directory `/home/subha/src/pymoose2/scheduling'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/scheduling'
make[1]: Entering directory `/home/subha/src/pymoose2/builtins'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/builtins'
make[1]: Entering directory `/home/subha/src/pymoose2/kinetics'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/kinetics'
make[1]: Entering directory `/home/subha/src/pymoose2/ksolve'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/ksolve'
make[1]: Entering directory `/home/subha/src/pymoose2/regressionTests'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/regressionTests'
make[1]: Entering directory `/home/subha/src/pymoose2/utility'
make[1]: Nothing to be done for `default'.
make[1]: Leaving directory `/home/subha/src/pymoose2/utility'
make[1]: Entering directory `/home/subha/src/pymoose2/pymoose'
g++ -g -pthread -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_GSL -DPYMOOSE -fPIC -fno-strict-aliasing -I/usr/include/python2.6  -I../basecode -I../msg -I../shell   -c -o moosemodule.o moosemodule.cpp
moosemodule.cpp: In function ‘PyObject* _pymoose_setCwe(PyObject*, PyObject*)’:
moosemodule.cpp:1133: warning: deprecated conversion from string constant to ‘char*’
g++ -g -pthread -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_GSL -DPYMOOSE -fPIC -fno-strict-aliasing -I/usr/include/python2.6  -I../basecode -I../msg -I../shell   -c -o pymoose_Neutral.o pymoose_Neutral.cpp
ld -r -o pymoose.o moosemodule.o pymoose_Neutral.o 
make[1]: Leaving directory `/home/subha/src/pymoose2/pymoose'
All Libs compiled
g++ -shared  -g -pthread -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_GSL -DPYMOOSE -fPIC -fno-strict-aliasing -I/usr/include/python2.6  -o _moose.so basecode/basecode.o msg/msg.o shell/shell.o biophysics/biophysics.o randnum/randnum.o scheduling/scheduling.o builtins/builtins.o kinetics/kinetics.o ksolve/ksolve.o regressionTests/rt.o utility/utility.o  pymoose/pymoose.o -ldl -lm -L/usr/lib -lgsl -lgslcblas -lpython2.6 
make -C pymoose
make[1]: Entering directory `/home/subha/src/pymoose2/pymoose'
make[1]: `pymoose.o' is up to date.
make[1]: Leaving directory `/home/subha/src/pymoose2/pymoose'

Compilation finished at Fri Mar 25 16:51:31
