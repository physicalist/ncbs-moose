#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _mesh.o

OBJ = \
	ChemMesh.o	\
	MeshEntry.o	\
	CylMesh.o	\
	CubeMesh.o	\
	Boundary.o	\
	Stencil.o	\
	testMesh.o	\

HEADERS = \
	../basecode/header.h \


default: $(TARGET)

$(OBJ)	: $(HEADERS)
MeshEntry.o:	MeshEntry.h
Boundary.o:	Boundary.h
ChemMesh.o:	ChemMesh.h MeshEntry.h Boundary.h ../ksolve/RateTerm.h ../ksolve/FuncTerm.h ../ksolve/KinSparseMatrix.h ../ksolve/Stoich.h
CylMesh.o:	ChemMesh.h CylMesh.h MeshEntry.h Boundary.h
CubeMesh.o:	ChemMesh.h CubeMesh.h MeshEntry.h Boundary.h
Stencil.o:	Stencil.h
testMesh.o:	MeshEntry.h ChemMesh.h CylMesh.h Boundary.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I.. -I../basecode $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*

