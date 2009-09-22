/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Geometry>

#include <vector>
#include <algorithm>
#include <math.h>

#include "GLCompartmentCylinder.h"

GLCompartmentCylinder::GLCompartmentCylinder( osg::Vec3 position, osg::Quat quatRotation, 
					      double height, double radius, double incrementAngle )
	:
	position_( position ),
	quatRotation_( quatRotation ),
	height_( height ),
	radius_( radius ),
	incrementAngle_( incrementAngle ),
	isLeftEndClosed( false ),
	isRightEndClosed( false ),
	ringRight( new osg::Vec3Array ),
	ringLeft( new osg::Vec3Array )
{ 
	std::vector< double > angles;
	for ( double i = 0; i <= 360 - incrementAngle_; i += incrementAngle_ )
		angles.push_back( 2*M_PI*i / 360 );
	angles.push_back( 0 );
    
	for ( int i = 0; i < angles.size(); ++i )
	{
		ringRight->push_back( rotateTranslatePoint( osg::Vec3( radius_*cos(angles[i]),
								       radius_*sin(angles[i]),
								       height_/2 ),
							    quatRotation_,
							    position_) );
	}

	for (int i = 0; i < angles.size(); ++i)
	{
		ringLeft->push_back( rotateTranslatePoint( osg::Vec3( radius_*cos(angles[i]),
								      radius_*sin(angles[i]),
								      -height_/2 ),
							    quatRotation_,
							    position_) );
	}
		
	cylGeometry_ = new osg::Geometry;
	cylVertices_ = new osg::Vec3Array;
	cylNormals_ = new osg::Vec3Array;

	for ( int j = 0; j < angles.size() - 1; ++j )
	{
		cylVertices_->push_back( ( *ringRight )[j+1] );
		cylVertices_->push_back( ( *ringRight )[j]   );
		cylVertices_->push_back( ( *ringLeft )[j]    );
		cylVertices_->push_back( ( *ringLeft )[j+1]  );    
	}
	cylGeometry_->setVertexArray( cylVertices_ );

	for ( int j = 0; j < angles.size() - 1; ++j )
	{
		osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );

		cylFaces->push_back( j*4 + 0 );
		cylFaces->push_back( j*4 + 1 );
		cylFaces->push_back( j*4 + 2 );
		cylFaces->push_back( j*4 + 3 );

		cylGeometry_->addPrimitiveSet( cylFaces );

		cylNormals_->push_back(makeNormal( ( *cylVertices_ )[j*4+0],
						   ( *cylVertices_ )[j*4+1],
						   ( *cylVertices_ )[j*4+2] ));
	}

	cylGeometry_->setNormalArray( cylNormals_ );
	cylGeometry_->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );
}

GLCompartmentCylinder::~GLCompartmentCylinder()
{
	ringRight = NULL;
	ringLeft = NULL;
	cylGeometry_ = NULL;
	cylVertices_ = NULL;
	cylNormals_ = NULL;
}

osg::ref_ptr< osg::Geometry > GLCompartmentCylinder::getGeometry()
{
	return cylGeometry_;
}
 
int GLCompartmentCylinder::getCompartmentType()
{
	return CYLINDER;
}

void GLCompartmentCylinder::setColor( osg::Vec4 color )
{
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( color );

	cylGeometry_->setColorArray( colors );
	cylGeometry_->setColorBinding( osg::Geometry::BIND_OVERALL );
}

osg::Vec3 GLCompartmentCylinder::makeNormal( const osg::Vec3& P1, const osg::Vec3& P2, const osg::Vec3& P3 )
{
	osg::Vec3 U = osg::Vec3( P2[0]-P1[0], P2[1]-P1[1], P2[2]-P1[2] );
	osg::Vec3 V = osg::Vec3( P3[0]-P1[0], P3[1]-P1[1], P3[2]-P1[2] );

	osg::Vec3 Normal;

	Normal[0] = U[1]*V[2] - U[2]*V[1];
	Normal[1] = U[2]*V[0] - U[0]*V[2];
	Normal[2] = U[0]*V[1] - U[1]*V[0];

	double mag = sqrt( Normal[0]*Normal[0] + Normal[1]*Normal[1] + Normal[2]*Normal[2] );

	Normal[0] /= mag;
	Normal[1] /= mag;
	Normal[2] /= mag;

	return Normal;
}

void GLCompartmentCylinder::addHemisphericalCap( bool leftEndP )
{
	// draw an approximation of a hemisphere at the given end
	int oldSizeCylVertices = cylVertices_->size();
    
	std::vector< double > angles;
	for ( double i = 0; i <= 360 - incrementAngle_; i += incrementAngle_ )
		angles.push_back( 2*M_PI*i / 360 );
	angles.push_back( 0 );
    
	double neighbourSign;
	if ( leftEndP )
	{
		neighbourSign = -1;
	}
	else
	{
		neighbourSign = 1;
	}

	// add vertex at tip first
	cylVertices_->push_back( rotateTranslatePoint( osg::Vec3(0, 0, neighbourSign * (radius_ + height_/2) ),
						       quatRotation_,
						       position_ ) );
	for (int i = 0; i < angles.size()-1; ++i)
	{
		osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

		cylVertices_->push_back( rotateTranslatePoint( osg::Vec3( radius_ * cos( angles[i] ),
									  radius_ * sin( angles[i] ),
									  neighbourSign * height_/2 ),
							       quatRotation_,
							       position_ ) );

		cylVertices_->push_back( rotateTranslatePoint( osg::Vec3( radius_ * cos( angles[i+1] ),
									  radius_ * sin( angles[i+1] ),
									  neighbourSign * height_/2  ),
							       quatRotation_,
							       position_ ) );

		int j;
		for ( j = 1; j <= 9; ++j )
		{
	  
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );

			cylVertices_->push_back( rotateTranslatePoint( osg::Vec3( radius_ * sin( acos(j*0.1) ) * cos( angles[i] ),
										  radius_ * sin( acos(j*0.1) ) * sin( angles[i] ),
										  neighbourSign * ( (j*0.1) * radius_ + height_/2 ) ),
								       quatRotation_,
								       position_ ) );

			cylVertices_->push_back( rotateTranslatePoint( osg::Vec3( radius_ * sin( acos(j*0.1) ) * cos( angles[i+1] ),
										  radius_ * sin( acos(j*0.1) ) * sin( angles[i+1] ),
										  neighbourSign * ( (j*0.1) * radius_ + height_/2 ) ),
								       quatRotation_,
								       position_ ) );

			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 1 + 2*(j-1) ); // 20 == 2 + 9*2 ; duplicate vertices on selfRing + two vertices per face added
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 0 + 2*(j-1) );
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1) );
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1) );

			cylGeometry_->addPrimitiveSet( cylFaces );

			osg::Vec3 normal = makeNormal( ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 1 + 2*(j-1)],
						       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 0 + 2*(j-1)],
						       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1)] );
			cylNormals_->push_back(osg::Vec3( normal[0] * -1 * neighbourSign,
							  normal[1] * -1 * neighbourSign,
							  normal[2] * -1 * neighbourSign ) );
	
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0);


		}

		j = 9;
	
		cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
		cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1) );
		cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1) );
		cylFaces->push_back( oldSizeCylVertices );

		cylGeometry_->addPrimitiveSet( cylFaces );

		osg::Vec3 normal = makeNormal( ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1)],
					       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1)],
					       ( *cylVertices_ )[oldSizeCylVertices] );
		cylNormals_->push_back( osg::Vec3( normal[0] * -1 * neighbourSign,
						   normal[1] * -1 * neighbourSign,
						   normal[2] * -1 * neighbourSign ) );
	}
}

bool GLCompartmentCylinder::addHalfJointToNeighbour( GLCompartmentCylinder* neighbour )
{
	// Helper function classes
	class distance_
	{
	public:
		double operator() ( osg::Vec3& P1, osg::Vec3& P2 )
		{
			return sqrt( pow(P1[0]-P2[0], 2) + pow(P1[1]-P2[1], 2) + pow(P1[2]-P2[2], 2) );
		}
	} distance;

	class midpoint_
	{
	public:
		osg::Vec3 operator() ( osg::Vec3& P1, osg::Vec3& P2 )
		{
			return osg::Vec3( mean(P1[0], P2[0]), mean(P1[1], P2[1]), mean(P1[2], P2[2]) );
		}
	private:
		double mean( double x, double y )
		{
			return (x + y) / 2;
		}
	} midpoint;

	// Find closest ring of neighbour by choosing the point, from the points with the highest y-ordinates
	// in each of the neighbour's two rings, that is closer to the point with the highest y-ordinate in selfRing.
	//
	// The choice of constraining the y-ordinate as opposed to choosing the closest points is arbitrary, but it simplifies
	// our task and the differences between ring distances are expected to be large enough that this shouldn't matter.

	std::vector< double > vecY; // temporary
	osg::Vec3Array* selfRing;      // this will be made to point to self's ring closest to the neighbour
	std::vector< double >::size_type iMaxYSelfRing;
	osg::Vec3Array* neighbourRing; // this will be made to point to the neighbour's ring closest to self
	std::vector< double >::size_type iMaxYNeighbourRing;
	
	osg::Vec3Array* selfRingRight = ringRight;
	for ( int i = 0; i < selfRingRight->size(); ++i )
		vecY.push_back( ( *selfRingRight )[i][1] );
	std::vector< double >::size_type iMaxYSelfRingRight = std::max_element(vecY.begin(), vecY.end()) - vecY.begin();

	vecY.clear();
	osg::Vec3Array* selfRingLeft = ringLeft;
	for ( int i = 0; i < selfRingLeft->size(); ++i )
		vecY.push_back( ( *selfRingLeft )[i][1] );
	std::vector< double >::size_type iMaxYSelfRingLeft = std::max_element(vecY.begin(), vecY.end()) - vecY.begin();

	vecY.clear();
	osg::Vec3Array* neighbourRingRight = neighbour->ringRight;
	for ( int i = 0; i < neighbourRingRight->size(); ++i )
		vecY.push_back( ( *neighbourRingRight )[i][1] );
	std::vector< double >::size_type iMaxYNeighbourRingRight = std::max_element(vecY.begin(), vecY.end()) - vecY.begin();
    
	vecY.clear();
	osg::Vec3Array* neighbourRingLeft = neighbour->ringLeft;
	for ( int i = 0; i < neighbourRingLeft->size(); ++i )
		vecY.push_back( ( *neighbourRingLeft )[i][1] );
	std::vector< double >::size_type iMaxYNeighbourRingLeft = std::max_element(vecY.begin(), vecY.end()) - vecY.begin();

	// leftLeft == left ring of self against left ring of neighbour, and so on...
	double leftLeftDistance = distance( ( *selfRingLeft )[iMaxYSelfRingLeft], ( *neighbourRingLeft )[iMaxYNeighbourRingLeft] );
	double leftRightDistance = distance( ( *selfRingLeft )[iMaxYSelfRingLeft], ( *neighbourRingRight )[iMaxYNeighbourRingRight] );
	double rightLeftDistance = distance( ( *selfRingRight )[iMaxYSelfRingRight], ( *neighbourRingLeft )[iMaxYNeighbourRingLeft] );
	double rightRightDistance = distance( ( *selfRingRight )[iMaxYSelfRingRight], ( *neighbourRingRight )[iMaxYNeighbourRingRight] );

	double leftClosestDistance = std::min( leftLeftDistance, leftRightDistance );
	double rightClosestDistance = std::min( rightLeftDistance, rightRightDistance );
	
	bool isLeftSelfRingCloser;

	if ( leftClosestDistance <= rightClosestDistance )
	{
		isLeftSelfRingCloser = true;

		if ( leftLeftDistance <= leftRightDistance )
		{
			selfRing = selfRingLeft;
			iMaxYSelfRing = iMaxYSelfRingLeft;
			neighbourRing = neighbourRingLeft;
			iMaxYNeighbourRing = iMaxYNeighbourRingLeft;
		}
		else
		{
			selfRing = selfRingLeft;
			iMaxYSelfRing = iMaxYSelfRingLeft;
			neighbourRing = neighbourRingRight;
			iMaxYNeighbourRing = iMaxYNeighbourRingRight;
		}
	}
	else
	{
		isLeftSelfRingCloser = false;

		if ( rightLeftDistance <= rightRightDistance )
		{
			selfRing = selfRingRight;
			iMaxYSelfRing = iMaxYSelfRingRight;
			neighbourRing = neighbourRingLeft;
			iMaxYNeighbourRing = iMaxYNeighbourRingLeft;
		}
		else
		{
			selfRing = selfRingRight;
			iMaxYSelfRing = iMaxYSelfRingRight;
			neighbourRing = neighbourRingRight;
			iMaxYNeighbourRing = iMaxYNeighbourRingRight;
		}
	}

	// Now that the neighbour's nearest ring has been found, we add new vertices (and faces)
	// at the midpoints between corresponding points of selfRing and the neighbour's ring.

	int iDiff = iMaxYNeighbourRing - iMaxYSelfRing;
	int oldSizeCylVertices = cylVertices_->size();

	for ( int i = 0; i < selfRing->size()-1; ++i )
	{
		// NOTE: Because selfRing's vertices already exist, we will be adding some duplicate vertices.
     
		cylVertices_->push_back( ( *selfRing )[i] );
		cylVertices_->push_back( ( *selfRing )[i+1] );
		cylVertices_->push_back( midpoint( ( *neighbourRing )[ (i+1+iDiff) % selfRing->size() ],
						  ( *selfRing )[i+1] ) );
		cylVertices_->push_back( midpoint( ( *neighbourRing )[ (i+iDiff) % selfRing->size() ],
						   ( *selfRing )[i] ) );
	}

	for ( int j = 0; j < selfRing->size()-1; ++j )
	{
		osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );

		if ( isLeftSelfRingCloser )
		{
			cylFaces->push_back( oldSizeCylVertices + j*4 + 0 ); // faces must be constructed anticlockwise from the front
			cylFaces->push_back( oldSizeCylVertices + j*4 + 1 );
			cylFaces->push_back( oldSizeCylVertices + j*4 + 2 );
			cylFaces->push_back( oldSizeCylVertices + j*4 + 3 );
		}
		else
		{
			cylFaces->push_back( oldSizeCylVertices + j*4 + 3 );
			cylFaces->push_back( oldSizeCylVertices + j*4 + 2 );
			cylFaces->push_back( oldSizeCylVertices + j*4 + 1 );
			cylFaces->push_back( oldSizeCylVertices + j*4 + 0 );
		}

		cylGeometry_->addPrimitiveSet( cylFaces );

		if ( isLeftSelfRingCloser )
		{
			osg::Vec3 normal = makeNormal( ( *cylVertices_ )[oldSizeCylVertices + j*4 + 0],
						       ( *cylVertices_ )[oldSizeCylVertices + j*4 + 1],
						       ( *cylVertices_ )[oldSizeCylVertices + j*4 + 2] );
			cylNormals_->push_back( osg::Vec3( -normal[0], -normal[1], -normal[2] ) );
		}
		else
		{
			cylNormals_->push_back( makeNormal( ( *cylVertices_ )[ oldSizeCylVertices + j*4 + 0 ],
							    ( *cylVertices_ )[ oldSizeCylVertices + j*4 + 1 ],
							    ( *cylVertices_ )[ oldSizeCylVertices + j*4 + 2 ] ) );
		}
	}
	return isLeftSelfRingCloser;
}
