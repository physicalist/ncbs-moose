/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SparseMatrix.h"
#include "ElementValueFinfo.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "Stencil.h"
#include "ChemMesh.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
#include "SparseMatrix.h"
#include "NeuroStencil.h"
#include "NeuroMesh.h"
#include "CylMesh.h"
#include "../utility/numutil.h"
const Cinfo* CylMesh::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< CylMesh, double > x0(
			"x0",
			"x coord of one end",
			&CylMesh::setX0,
			&CylMesh::getX0
		);
		static ValueFinfo< CylMesh, double > y0(
			"y0",
			"y coord of one end",
			&CylMesh::setY0,
			&CylMesh::getY0
		);
		static ValueFinfo< CylMesh, double > z0(
			"z0",
			"z coord of one end",
			&CylMesh::setZ0,
			&CylMesh::getZ0
		);
		static ValueFinfo< CylMesh, double > r0(
			"r0",
			"Radius of one end",
			&CylMesh::setR0,
			&CylMesh::getR0
		);
		static ValueFinfo< CylMesh, double > x1(
			"x1",
			"x coord of other end",
			&CylMesh::setX1,
			&CylMesh::getX1
		);
		static ValueFinfo< CylMesh, double > y1(
			"y1",
			"y coord of other end",
			&CylMesh::setY1,
			&CylMesh::getY1
		);
		static ValueFinfo< CylMesh, double > z1(
			"z1",
			"z coord of other end",
			&CylMesh::setZ1,
			&CylMesh::getZ1
		);
		static ValueFinfo< CylMesh, double > r1(
			"r1",
			"Radius of other end",
			&CylMesh::setR1,
			&CylMesh::getR1
		);
		static ElementValueFinfo< CylMesh, vector< double > > coords(
			"coords",
			"All the coords as a single vector: x0 y0 z0  x1 y1 z1  r0 r1 lambda",
			&CylMesh::setCoords,
			&CylMesh::getCoords
		);

		static ValueFinfo< CylMesh, double > lambda(
			"lambda",
			"Length constant to use for subdivisions"
			"The system will attempt to subdivide using compartments of"
			"length lambda on average. If the cylinder has different end"
			"diameters r0 and r1, it will scale to smaller lengths"
			"for the smaller diameter end and vice versa."
			"Once the value is set it will recompute lambda as "
			"totLength/numEntries",
			&CylMesh::setLambda,
			&CylMesh::getLambda
		);

		static ReadOnlyValueFinfo< CylMesh, double > totLength(
			"totLength",
			"Total length of cylinder",
			&CylMesh::getTotLength
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		// Field Elements
		//////////////////////////////////////////////////////////////

	static Finfo* cylMeshFinfos[] = {
		&x0,			// Value
		&y0,			// Value
		&z0,			// Value
		&r0,			// Value
		&x1,			// Value
		&y1,			// Value
		&z1,			// Value
		&r1,			// Value
		&lambda,			// Value
		&coords,		// Value
		&totLength,		// ReadOnlyValue
	};

	static Cinfo cylMeshCinfo (
		"CylMesh",
		ChemMesh::initCinfo(),
		cylMeshFinfos,
		sizeof( cylMeshFinfos ) / sizeof ( Finfo* ),
		new Dinfo< CylMesh >()
	);

	return &cylMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* cylMeshCinfo = CylMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
CylMesh::CylMesh()
	:
		numEntries_( 1 ),
		useCaps_( 0 ),
		isToroid_( false ),
		x0_( 0.0 ),
		y0_( 0.0 ),
		z0_( 0.0 ),
		x1_( 1.0 ),
		y1_( 0.0 ),
		z1_( 0.0 ),
		r0_( 1.0 ),
		r1_( 1.0 ),
		lambda_( 1.0 ),
		totLen_( 1.0 ),
		rSlope_( 0.0 ),
		lenSlope_( 0.0 )
{
	;
}

CylMesh::~CylMesh()
{
	;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////

/**
 * This assumes that lambda is the quantity to preserve, over numEntries.
 * So when the compartment changes size, so does numEntries. Lambda will
 * be fine-tuned to be a clean multiple.
 */
void CylMesh::updateCoords()
{
	double temp = sqrt( 
		( x1_ - x0_ ) * ( x1_ - x0_ ) + 
		( y1_ - y0_ ) * ( y1_ - y0_ ) + 
		( z1_ - z0_ ) * ( z1_ - z0_ )
	);

	if ( doubleEq( temp, 0.0 ) ) {
		cout << "Error: CylMesh::updateCoords:\n"
		"total length of compartment = 0 with these parameters\n";
		return;
	}
	totLen_ = temp;


	temp = totLen_ / lambda_;
	if ( temp < 1.0 ) {
		lambda_ = totLen_;
		numEntries_ = 1;
	} else {
		numEntries_ = static_cast< unsigned int >( round ( temp ) );
		lambda_ = totLen_ / numEntries_;
	}
	rSlope_ = ( r1_ - r0_ ) / numEntries_;
	lenSlope_ = lambda_ * rSlope_ * 2 / ( r0_ + r1_ );

	// dx2_[0] = lambda_;
	// dx2_[1] = lambda_;
	buildStencil();
}

void CylMesh::setX0( double v )
{
	x0_ = v;
	updateCoords();
}

double CylMesh::getX0() const
{
	return x0_;
}

void CylMesh::setY0( double v )
{
	y0_ = v;
	updateCoords();
}

double CylMesh::getY0() const
{
	return y0_;
}

void CylMesh::setZ0( double v )
{
	z0_ = v;
	updateCoords();
}

double CylMesh::getZ0() const
{
	return z0_;
}

void CylMesh::setR0( double v )
{
	r0_ = v;
	updateCoords();
}

double CylMesh::getR0() const
{
	return r0_;
}


void CylMesh::setX1( double v )
{
	x1_ = v;
	updateCoords();
}

double CylMesh::getX1() const
{
	return x1_;
}

void CylMesh::setY1( double v )
{
	y1_ = v;
	updateCoords();
}

double CylMesh::getY1() const
{
	return y1_;
}

void CylMesh::setZ1( double v )
{
	z1_ = v;
	updateCoords();
}

double CylMesh::getZ1() const
{
	return z1_;
}

void CylMesh::setR1( double v )
{
	r1_ = v;
	updateCoords();
}

double CylMesh::getR1() const
{
	return r1_;
}

void CylMesh::innerSetCoords( const vector< double >& v )
{
	x0_ = v[0];
	y0_ = v[1];
	z0_ = v[2];

	x1_ = v[3];
	y1_ = v[4];
	z1_ = v[5];

	r0_ = v[6];
	r1_ = v[7];

	lambda_ = v[8];

	updateCoords();
}

void CylMesh::setCoords( const Eref& e, const Qinfo* q, vector< double > v )
{
	if ( v.size() < 9 ) {
		cout << "CylMesh::setCoords: Warning: size of argument vec should be >= 9, was " << v.size() << endl;
	}
	innerSetCoords( v );
	transmitChange( e, q );
}

vector< double > CylMesh::getCoords( const Eref& e, const Qinfo* q ) const
{
	vector< double > ret( 9 );

	ret[0] = x0_;
	ret[1] = y0_;
	ret[2] = z0_;

	ret[3] = x1_;
	ret[4] = y1_;
	ret[5] = z1_;

	ret[6] = r0_;
	ret[7] = r1_;

	ret[8] = lambda_;
	return ret;
}


void CylMesh::setLambda( double v )
{
	lambda_ = v;
	updateCoords();
}

double CylMesh::getLambda() const
{
	return lambda_;
}


double CylMesh::getTotLength() const
{
	return totLen_;
}

unsigned int CylMesh::innerGetDimensions() const
{
	return 3;
}

//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/// Virtual function to return MeshType of specified entry.
unsigned int CylMesh::getMeshType( unsigned int fid ) const
{
	if ( !isToroid_ && useCaps_ && ( fid == 0 || fid == numEntries_ - 1 ) )
		return SPHERE_SHELL_SEG;

	return CYL;
}

/// Virtual function to return dimensions of specified entry.
unsigned int CylMesh::getMeshDimensions( unsigned int fid ) const
{
	return 3;
}

/**
 * lambda = length constant for diffusive spread
 * len = length of each mesh entry
 * totLen = total length of cylinder
 * lambda = k * r^2
 * Each entry has the same number of lambdas, L = len/lambda.
 * Thinner entries have shorter lambda.
 * This gives a moderately nasty quadratic.
 * However, as len(i) is prop to lambda(i),
 * and lambda(i) is prop to r(i)^2
 * and the cyl-mesh is assumed a gently sloping cone
 * we get len(i) is prop to (r0 + slope.x)^2
 * and ignoring the 2nd-order term we have
 * len(i) is approx proportional to x position.
 *
 * dr/dx = (r1-r0)/len
 * ri = r0 + i * dr/dx
 * r(i+1)-ri = (r1-r0)/numEntries
 * len = k * r^2
 * we get k from integ_r0,r1( len.dr ) = totLen
 * So k.r^3/3 | r0, r1 = totLen
 * => k/3 * ( r1^3 - r0^3) = totLen
 * => k = 3 * totLen / (r1^3 - r0^3);
 * This is bad if r1 == r0, and is generally unpleasant.
 * 
 * Simple definition of rSlope:
 * rSlope is measured per meshEntry, not per length:
 * rSlope = ( r1 - r0 ) / numEntries;
 * Let's just compute len0 from r0 and lambda.
 * len0/lambda = 2 * r0 / (r0 + r1)
 * so len0 = lambda * 2 * r0 / (r0 + r1)
 * and dlen/dx = lenSlope = lambda * rSlope * 2/(r0 + r1)
 *
 * Drop the following calculations:
 * // dlen/dx = dr/dx * dlen/dr = ( (r1-r0)/len ) * 2k.r
 * // To linearize, let 2r = r0 + r1.
 * // so dlen/dx = ( (r1-r0)/len ) * k * ( r0 + r1 )
 * // len(i) = len0 + i * dlen/dx
 * // len0 = totLen/numEntries - ( numEntries/2 ) * dlen/dx 
 */

/// Virtual function to return volume of mesh Entry.
double CylMesh::getMeshEntrySize( unsigned int fid ) const
{
 	double len0 = lambda_ * 2 * r0_ / ( r0_ + r1_ );

	double ri = r0_ + (fid + 0.5) * rSlope_;
	double leni = len0 + ( fid + 0.5 ) * lenSlope_;

	return leni * ri * ri * PI;
}

/// Virtual function to return coords of mesh Entry.
/// For Cylindrical mesh, coords are x1y1z1 x2y2z2 r0 r1 phi0 phi1
vector< double > CylMesh::getCoordinates( unsigned int fid ) const
{
	vector< double > ret(10, 0.0);
 	double len0 = lambda_ * 2 * r0_ / ( r0_ + r1_ );
 	// double len0 = lambda_ * 2 * ( r0_ + rSlope_ / 0.5) / ( r0_ + r1_ );
	double lenStart = len0 + lenSlope_ * 0.5;

	double axialStart = fid * lenStart + ( ( fid * (fid - 1 ) )/2 ) * lenSlope_;
		// fid * totLen_/numEntries_ + (fid - frac ) * lenSlope_;
	double axialEnd = 
		(fid + 1) * lenStart + ( ( fid * (fid + 1 ) )/2 ) * lenSlope_;
		// (fid + 1) * totLen_/numEntries_ + (fid - frac + 1.0) * lenSlope_;

	ret[0] = x0_ + (x1_ - x0_ ) * axialStart/totLen_;
	ret[1] = y0_ + (y1_ - y0_ ) * axialStart/totLen_;
	ret[2] = z0_ + (z1_ - z0_ ) * axialStart/totLen_;

	ret[3] = x0_ + (x1_ - x0_ ) * axialEnd/totLen_;
	ret[4] = y0_ + (y1_ - y0_ ) * axialEnd/totLen_;
	ret[5] = z0_ + (z1_ - z0_ ) * axialEnd/totLen_;

	ret[6] = r0_ + fid * rSlope_;
	ret[7] = r0_ + (fid + 1.0) * rSlope_;

	ret[8] = 0;
	ret[9] = 0;
	
	return ret;
}
/// Virtual function to return info on Entries connected to this one
vector< unsigned int > CylMesh::getNeighbors( unsigned int fid ) const
{
	const double* entry;
	const unsigned int *colIndex;
	vector< unsigned int > ret;
	unsigned int n = m_.getRow( fid, &entry, &colIndex );
	ret.insert( ret.end(), colIndex, colIndex + n );
	return ret;
		/*
	if ( numEntries_ <= 1 )
		return vector< unsigned int >( 0 );
	
	if ( isToroid_ ) {
		vector< unsigned int > ret( 2, 0 );
		ret[0] = ( fid == 0 ) ? numEntries_ - 1 : fid - 1;
		ret[1] = ( fid == numEntries_ - 1 ) ? 0 : fid + 1;
		return ret;
	}

	if ( fid == 0 )
		return vector< unsigned int >( 1, 1 );
	else if ( fid == numEntries_ - 1 )
		return vector< unsigned int >( 1, numEntries_ - 2 );
		
	vector< unsigned int > ret( 2, 0 );
	ret[0] = fid - 1;
	ret[1] = fid + 1;
	return ret;	
	*/
}

/// Virtual function to return diffusion X-section area for each neighbor
vector< double > CylMesh::getDiffusionArea( unsigned int fid ) const
{
	if ( numEntries_ <= 1 )
		return vector< double >( 0 );

	double rlow = r0_ + fid * rSlope_;
	double rhigh = r0_ + (fid + 1.0) * rSlope_;

	if ( fid == 0 ) {
		if ( isToroid_ ) {
			vector < double > ret( 2 );
			ret[0] = rlow * rlow * PI;
			ret[1] = rhigh * rhigh * PI;
			return ret;
		} else {
			return vector < double >( 1, rhigh * rhigh * PI );
		}
	}

	if ( fid == (numEntries_ - 1 ) ) {
		if ( isToroid_ ) {
			vector < double > ret( 2 );
			ret[0] = rlow * rlow * PI;
			ret[1] = r0_ * r0_ * PI; // Wrapping around
			return ret;
		} else {
			return vector < double >( 1, rlow * rlow * PI );
		}
	}
	vector< double > ret( 2 );
	ret[0] = rlow * rlow * PI;
	ret[1] = rhigh * rhigh * PI;
	return ret;
}

/// Virtual function to return scale factor for diffusion. 1 here.
vector< double > CylMesh::getDiffusionScaling( unsigned int fid ) const
{
	if ( numEntries_ <= 1 )
		return vector< double >( 0 );

	if ( !isToroid_ && ( fid == 0 || fid == (numEntries_ - 1) ) )
		return vector< double >( 1, 1.0 );

	return vector< double >( 2, 1.0 );
}

/// Virtual function to return volume of mesh Entry, including
/// for diffusively coupled voxels from other solvers.
double CylMesh::extendedMeshEntrySize( unsigned int fid ) const
{
	if ( fid < numEntries_ ) {
		return getMeshEntrySize( fid );
	} else {
		assert( 0 ); // Doesn't work yet.
		return 0;
	}
}

// Not yet implemented.
void CylMesh::clearExtendedMeshEntrySize()
{
	;
}



//////////////////////////////////////////////////////////////////
// Dest funcsl
//////////////////////////////////////////////////////////////////

/// More inherited virtual funcs: request comes in for mesh stats
void CylMesh::innerHandleRequestMeshStats( const Eref& e, const Qinfo* q, 
		const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo
	)
{
	vector< double > ret( size_ / numEntries_ ,1 );
	meshStatsFinfo->send( e, q->threadNum(), 1, ret );
}

void CylMesh::innerHandleNodeInfo(
			const Eref& e, const Qinfo* q, 
			unsigned int numNodes, unsigned int numThreads )
{
	unsigned int numEntries = numEntries_;
	vector< double > vols( numEntries, size_ / numEntries );
	vector< unsigned int > localEntries( numEntries );
	vector< vector< unsigned int > > outgoingEntries;
	vector< vector< unsigned int > > incomingEntries;
	double oldvol = getMeshEntrySize( 0 );
	meshSplit()->send( e, q->threadNum(), 
		oldvol,
		vols, localEntries,
		outgoingEntries, incomingEntries );
}
//////////////////////////////////////////////////////////////////

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int CylMesh::innerGetNumEntries() const
{
	return numEntries_;
}

/**
 * Inherited virtual func. Assigns number of MeshEntries.
 */
void CylMesh::innerSetNumEntries( unsigned int n )
{
	static const unsigned int WayTooLarge = 1000000;
	if ( n == 0 || n > WayTooLarge ) {
		cout << "Warning: CylMesh::innerSetNumEntries( " << n <<
		" ): out of range\n";
		return;
	}
	assert( n > 0 );
	numEntries_ = n;
	lambda_ = totLen_ / n;
	rSlope_ = ( r1_ - r0_ ) / numEntries_;
	lenSlope_ = lambda_ * rSlope_ * 2 / ( r0_ + r1_ );

	buildStencil();
}


void CylMesh::innerBuildDefaultMesh( const Eref& e, const Qinfo* q,
	double size, unsigned int numEntries )
{
	/// Cylinder with diameter = length.
	/// vol = size = pi.r^2.len. 
	/// So len = 2r, size = pi*r^2*2r = 2pi*r^3 so r = (size/2pi)^(1/3)
	double r = pow( ( size / ( PI * 2 ) ), 1.0 / 3 );
	vector< double > coords( 9, 0 );
	coords[3] = 2 * r;
	coords[6] = r;
	coords[7] = r;
	coords[8] = 2 * r / numEntries;
	setCoords( e, q, coords );

	/*
	Id meshEntry( e.id().value() + 1 );
	assert( 
		meshEntry.eref().data() == reinterpret_cast< char* >( lookupEntry( 0 ) )
	);
	vector< unsigned int > localIndices; // empty
	vector< double > vols( numEntries_, size_/numEntries_ );
	lookupEntry( 0 )->triggerRemesh( meshEntry.eref(), q->threadNum(), 
		0, localIndices, vols );
	*/
}

//////////////////////////////////////////////////////////////////
// Utility function to transmit any changes to target nodes.
//////////////////////////////////////////////////////////////////

void CylMesh::transmitChange( const Eref& e, const Qinfo* q )
{
	Id meshEntry( e.id().value() + 1 );
	assert( 
		meshEntry.eref().data() == reinterpret_cast< char* >( lookupEntry( 0 ) )
	);
	double oldvol = getMeshEntrySize( 0 );
	unsigned int totalNumEntries = numEntries_;
	unsigned int localNumEntries = totalNumEntries;
	unsigned int startEntry = 0;
	vector< unsigned int > localIndices( localNumEntries ); // empty
	for ( unsigned int i = 0; i < localNumEntries; ++i )
		localIndices[i] = i;
	vector< double > vols( localNumEntries, size_ / numEntries_ );
	vector< vector< unsigned int > > outgoingEntries; // [node#][Entry#]
	vector< vector< unsigned int > > incomingEntries; // [node#][Entry#]

	// This function updates the size of the FieldDataHandler for the 
	// MeshEntries.
	DataHandler* dh = meshEntry.element()->dataHandler();
	FieldDataHandlerBase* fdh = dynamic_cast< FieldDataHandlerBase* >( dh );
	assert( fdh );
	if ( totalNumEntries > fdh->getMaxFieldEntries() ) {
		fdh->setMaxFieldEntries( localNumEntries );
	}

	// This message tells the Stoich about the new mesh, and also about
	// how it communicates with other nodes.
	meshSplit()->fastSend( e, q->threadNum(), 
		oldvol,
		vols, localIndices, 
		outgoingEntries, incomingEntries );

	// This func goes down to the MeshEntry to tell all the pools and
	// Reacs to deal with the new mesh. They then update the stoich.
	lookupEntry( 0 )->triggerRemesh( meshEntry.eref(), q->threadNum(), 
		oldvol, startEntry, localIndices, vols );
}

//////////////////////////////////////////////////////////////////
// Utility function to set up Stencil for diffusion
//////////////////////////////////////////////////////////////////
void CylMesh::buildStencil()
{
	coreStencil_.clear();
	coreStencil_.setSize( numEntries_, numEntries_ );
	for ( unsigned int i = 0; i < numEntries_; ++i ) {
		double rLow = r0_ + i * rSlope_;
		double rHigh = r0_ + (i + 1.0) * rSlope_;
		double aLow = rLow * rLow * PI;
		double aHigh = rHigh * rHigh * PI;
		vector< double > entry;
		vector< unsigned int > colIndex;
		if ( i == 0 ) {
			colIndex.push_back( 1 );
			entry.push_back( aHigh / lambda_ );
			if ( isToroid_ ) {
				colIndex.push_back( numEntries_ - 1 );
				entry.push_back( aLow / lambda_ );
			}
		} else if ( i == numEntries_ - 1 ) {
			if ( isToroid_ ) {
				colIndex.push_back( 0 );
				if ( r0_ < r1_ )
					entry.push_back( r0_ * r0_ * PI / lambda_ );
				else
					entry.push_back( r1_ * r1_ * PI / lambda_ );
			}
			colIndex.push_back( numEntries_ - 2 );
			entry.push_back( aLow / lambda_ );
		} else { // Mostly it is in the middle.
			colIndex.push_back( i - 1 );
			entry.push_back( aLow / lambda_ );
			colIndex.push_back( i + 1 );
			entry.push_back( aHigh / lambda_ );
		}
		coreStencil_.addRow( i, entry, colIndex );
	}
		/*
	for ( unsigned int i = 0; i < stencil_.size(); ++i )
		delete stencil_[i];
	stencil_.resize( 1 );
	stencil_[0] = new LineStencil( lambda_ );
	*/
	m_ = coreStencil_;
}

unsigned int CylMesh::getStencil( unsigned int meshIndex,
			const double** entry, const unsigned int** colIndex ) const
{
	return m_.getRow( meshIndex, entry, colIndex );
}

void CylMesh::extendStencil( 
	   	const ChemMesh* other, const vector< VoxelJunction >& vj )
{
	assert( 0 ); // doesn't work yet.
}

void CylMesh::innerResetStencil()
{
	m_ = coreStencil_;
}

//////////////////////////////////////////////////////////////////
// Utility function for junctions
//////////////////////////////////////////////////////////////////

void CylMesh::matchMeshEntries( const ChemMesh* other,
	   vector< VoxelJunction >& ret ) const
{
	// This is seriously ugly, and what virtual funcs were meant to handle.
	// Dirty hack for now.
	const CylMesh* cyl = dynamic_cast< const CylMesh* >( other );
	if ( cyl ) {
		matchCylMeshEntries( cyl, ret );
	} else {
		const CubeMesh* cube = dynamic_cast< const CubeMesh* >( other );
		if ( cube ) {
			matchCubeMeshEntries( cube, ret );
		} else {
			const NeuroMesh* nm = dynamic_cast< const NeuroMesh* >( other );
			if ( nm ) {
				matchNeuroMeshEntries( nm, ret );
			} else {
				cout << "Warning:CylMesh::matchMeshEntries: "  <<
						" unknown mesh type\n";
			}
		}
	}
}

// Look for end-to-end diffusion, not sideways for now.
// Very straightforward but tedious because of the different permutations.
// Could readily add cylinder side to other end.
void CylMesh::matchCylMeshEntries( const CylMesh* other,
vector< VoxelJunction >& ret ) const
{
	const double EPSILON = 1e-3;
	ret.clear();
	// Should really estimate the distance from the centre of the smaller 
	// cylinder cap disk to the plane of the larger disk, provided it is
	// within the radius of the disk. The subsequent calculations are the
	// same though.
	double dr1 = 
			distance(x0_ - other->x0_, y0_ - other->y0_, z0_ - other->z0_ );
	double dr2 = 
			distance(x1_ - other->x1_, y1_ - other->y1_, z1_ - other->z1_ );
	double dr3 = 
			distance(x1_ - other->x0_, y1_ - other->y0_, z1_ - other->z0_ );
	double dr4 = 
			distance(x0_ - other->x1_, y0_ - other->y1_, z0_ - other->z1_ );

	if ( dr1 <= dr2 && dr1 <= dr3 && dr1 <= dr4 ) {
		if ( ( dr1/totLen_ < EPSILON && dr1/other->totLen_ < EPSILON ) ) {
			double xda;
			if ( r0_ < other->r0_ )
				xda = 2 * r0_ * r0_ * PI / ( lambda_ + other->lambda_ );
			else 
				xda = 2 * other->r0_ * other->r0_ * PI / 
						( lambda_ + other->lambda_ );
			ret.push_back( VoxelJunction( 0, 0, xda ) );
		}
	} else if ( dr2 <= dr3 && dr2 <= dr4 ) {
		if ( ( dr2/totLen_ < EPSILON && dr2/other->totLen_ < EPSILON ) ) {
			double xda;
			if ( r1_ < other->r1_ )
				xda = 2 * r1_ * r1_ * PI / ( lambda_ + other->lambda_ );
			else 
				xda = 2 * other->r1_ * other->r1_ * PI / 
						( lambda_ + other->lambda_ );
			ret.push_back( VoxelJunction( numEntries_ - 1, 
						other->numEntries_ - 1, xda ) );
		}
	} else if ( dr3 <= dr4 ) {
		if ( ( dr3/totLen_ < EPSILON && dr3/other->totLen_ < EPSILON ) ) {
			double xda;
			if ( r1_ < other->r0_ )
				xda = 2 * r1_ * r1_ * PI / ( lambda_ + other->lambda_ );
			else 
				xda = 2 * other->r0_ * other->r0_ * PI / 
						( lambda_ + other->lambda_ );
			ret.push_back( VoxelJunction( numEntries_ - 1, 0, xda ) );
		}
	} else {
		if ( ( dr4/totLen_ < EPSILON && dr4/other->totLen_ < EPSILON ) ) {
			double xda;
			if ( r1_ < other->r0_ )
				xda = 2 * r0_ * r0_ * PI / ( lambda_ + other->lambda_ );
			else 
				xda = 2 * other->r1_ * other->r1_ * PI / 
						( lambda_ + other->lambda_ );
			ret.push_back( VoxelJunction( 0, other->numEntries_ - 1, xda ));
		}
	}
}

void CylMesh::matchCubeMeshEntries( const CubeMesh* other,
vector< VoxelJunction >& ret ) const
{
}


void CylMesh::matchNeuroMeshEntries( const NeuroMesh* other,
vector< VoxelJunction >& ret ) const
{
}

void CylMesh::indexToSpace( unsigned int index,
			double& x, double& y, double& z ) const 
{
	if ( index < numEntries_ ) { 
		double k = ( index + 0.5 ) / static_cast< double >( numEntries_ );
		x = ( x1_ - x0_ ) * k + x0_;
		y = ( y1_ - y0_ ) * k + y0_;
		z = ( z1_ - z0_ ) * k + z0_;
	}
}

static double dotprd ( double x0, double y0, double z0, 
				double x1, double y1, double z1 )
{
		return x0 * x1 + y0 * y1 + z0 * z1;
}

double CylMesh::nearest( double x, double y, double z, 
				unsigned int& index ) const
{
	// Consider r0 = x0,y0,z0 and r1 = x1, y1, z1, and r = x,y,z.
	// Fraction along cylinder = k
	//
	// Then, point p along line from r0 to r1 is
	// p = k( r0-r1) + r1.
	//
	// Solving,
	// k = (r0 - r1).(r - r1) / (|r0-r1|^2)
	//
	
	double dist = distance( x1_ - x0_, y1_ - y0_, z1_ - z0_ );
	double k = dotprd( 
		x1_ - x0_, y1_ - y0_, z1_ - z0_,
		x - x0_, y - y0_, z - z0_ ) / ( dist * dist );

	// x2, y2, z2 are the coords of the nearest point.
	double x2 = k * (x1_ - x0_) + x0_;
	double y2 = k * (y1_ - y0_) + y0_;
	double z2 = k * (z1_ - z0_) + z0_;

	double ret = distance( x - x2, y - y2, z - z2 );
	if ( k < 0.0 ) {
		ret = -ret;
		index = 0;
	} else if ( k > 1.0 ) {
		ret = -ret;
		index = numEntries_ - 1;
	} else { // Inside length of cylinder, now is it inside radius?
		index = k * numEntries_;
		double ri = r0_ + (index + 0.5) * rSlope_;
		if ( ret > ri )
			ret = -ret;
	}
	return ret;
}
