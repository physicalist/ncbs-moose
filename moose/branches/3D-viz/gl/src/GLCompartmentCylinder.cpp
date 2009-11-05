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
	ringRight( new osg::Vec3Array ),
	ringLeft( new osg::Vec3Array ),
	position_( position ),
	quatRotation_( quatRotation ),
	height_( height ),
	radius_( radius ),
	incrementAngle_( incrementAngle ),
	isLeftEndClosed_( false ),
	isRightEndClosed_( false )
{ 
	std::vector< double > angles;
	for ( double i = 0; i <= 360 - incrementAngle_; i += incrementAngle_ )
		angles.push_back( 2*M_PI*i / 360 );
	angles.push_back( 0 );
    
	for ( unsigned int i = 0; i < angles.size(); ++i )
	{
		ringRight->push_back( rotateTranslatePoint( osg::Vec3( radius_*cos(angles[i]),
								       radius_*sin(angles[i]),
								       height_/2 ),
							    quatRotation_,
							    position_) );
	}

	for ( unsigned int i = 0; i < angles.size(); ++i)
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

	for ( unsigned int j = 0; j < angles.size() - 1; ++j )
	{
		cylVertices_->push_back( ( *ringRight )[j+1] );
		cylVertices_->push_back( ( *ringRight )[j]   );
		cylVertices_->push_back( ( *ringLeft )[j]    );
		cylVertices_->push_back( ( *ringLeft )[j+1]  );    
	}
	cylGeometry_->setVertexArray( cylVertices_ );

	for ( unsigned int j = 0; j < angles.size() - 1; ++j )
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
	osg::Vec4Array* colors_ = new osg::Vec4Array;

	colors_->push_back( color );

	cylGeometry_->setColorArray( colors_ );
	cylGeometry_->setColorBinding( osg::Geometry::BIND_OVERALL );
}

////////////////////////////////////////////////////////////////////////
// The following function, isPointInsideCylinder, is derived from     //
// http://www.flipcode.com/archives/Fast_Point-In-Cylinder_Test.shtml //
// The original code is in the public domain.			      //
////////////////////////////////////////////////////////////////////////
bool GLCompartmentCylinder::isPointInsideCylinder( osg::Vec3& testPoint )
{
	// p1 is the point at the center of the base-face of the cylinder.
	osg::Vec3 p1 = rotateTranslatePoint( osg::Vec3( 0, 0, -height_/2 ),
					     quatRotation_,
					     position_);

	// p2 is the point at the center of the top-face of the cylinder.
	osg::Vec3 p2 = rotateTranslatePoint( osg::Vec3( 0, 0, height_/2 ),
					     quatRotation_,
					     position_);
	
	// vector d goes from p1 to p2
	osg::Vec3 d = p2 - p1;
	
	// vector pd goes from p1 to testPoint
	osg::Vec3 pd = testPoint - p1;

	// Dot the d and pd vectors to see if point lies behind the 
	// cylinder cap at pt1.x, pt1.y, pt1.z

	double dot = pd * d;

	if ( dot < 0 || dot > pow( height_, 2 ) )
	{
		return false; // testPoint is not between base and top faces
	}
	else
	{
		// Distance squared to the cylinder axis
		double dsq = ( pd * pd ) - ( dot * dot ) / pow( height_, 2 );

		if ( dsq > pow (radius_, 2 ) )
		{
			return false; // testPoint is outside the radius
		}
	}

	return true;
}


void GLCompartmentCylinder::closeOpenEnds()
{
	if ( !isLeftEndClosed_ )
		addHemisphericalCap( true );

	if ( !isRightEndClosed_)
		addHemisphericalCap( false );
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
	for ( unsigned int i = 0; i < angles.size()-1; ++i)
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
		unsigned int j;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following function adds half a joint between self and neighbour; a corresponding call on the neighbour object    //
// will add the other half thus completing the joint. Thus all joints are composed of two half-joints, with each half   //
// belonging to the geometry of its closer compartment. This is useful in the case where the two compartments have      //
// different values for the attribute under observation and are colored differently; joints will then carry both colors //
// in equal parts.												        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GLCompartmentCylinder::addHalfJointToNeighbour( GLCompartmentCylinder* neighbour )
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

	osg::Vec3Array* selfRing;      // this will be made to point to self's ring closest to the neighbour
	std::vector< double >::size_type iMaxYSelfRing;
	osg::Vec3Array* neighbourRing; // this will be made to point to the neighbour's ring closest to self
	std::vector< double >::size_type iMaxYNeighbourRing;

	class getIMaxY_
	{
	public:
		std::vector< double >::size_type operator() ( osg::Vec3Array * vec )
		{
			vecY.clear();
		  
			for ( unsigned int i = 0; i < vec->size(); ++i )
			{
				vecY.push_back( ( *vec )[i][1] );
			}
			return std::max_element(vecY.begin(), vecY.end()) - vecY.begin();
		}
	private:
		std::vector< double > vecY;		
	} getIMaxY;
	

	osg::Vec3Array* selfRingRight = ringRight;
	osg::Vec3Array* selfRingLeft = ringLeft;
	osg::Vec3Array* neighbourRingRight = neighbour->ringRight;
	osg::Vec3Array* neighbourRingLeft = neighbour->ringLeft;

	std::vector< double >::size_type iMaxYSelfRingRight = getIMaxY( selfRingRight );
	std::vector< double >::size_type iMaxYSelfRingLeft = getIMaxY( selfRingLeft );
	std::vector< double >::size_type iMaxYNeighbourRingRight = getIMaxY( neighbourRingRight );
	std::vector< double >::size_type iMaxYNeighbourRingLeft = getIMaxY( neighbourRingLeft );

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

	
	if (isLeftSelfRingCloser)
	{
		isLeftEndClosed_ = true;
	}
	else
	{
		isRightEndClosed_ = true;
	}

	/*// Now that the neighbour's nearest ring has been found, we check if our closest ring lies
	// completely within the body of the neighbour or if the neighbour's lies completely within ours.
	// In either case, no half-joint is constructed.

	bool notAllPointsInside = false;

	for ( unsigned int i = 0; i < selfRing->size(); ++i )
	{
		if ( ! ( neighbour->isPointInsideCylinder( ( *selfRing )[i] ) ) )
		{
			notAllPointsInside = true;
		}
	}

	for ( unsigned int i = 0; i < neighbourRing->size(); ++i )
	{
		if ( ! ( isPointInsideCylinder( ( *neighbourRing )[i] ) ) )
		{
			notAllPointsInside = true;
		}
	}

	if ( ! notAllPointsInside )
	{
		return;
	 }*/

	// Now that we have verified that the half-joint will not be created completely within our own
	// body or within the neighbour's body, we can proceed with constructing it. 
	// We add new vertices (and faces) at the midpoints between corresponding points of selfRing
	// and the neighbour's ring.

	int iDiff = iMaxYNeighbourRing - iMaxYSelfRing;
	int oldSizeCylVertices = cylVertices_->size();

	for ( unsigned int i = 0; i < selfRing->size()-1; ++i )
	{
		// NOTE: Because selfRing's vertices already exist, we will be adding some duplicate vertices.
     
		cylVertices_->push_back( ( *selfRing )[i] );
		cylVertices_->push_back( ( *selfRing )[i+1] );
		cylVertices_->push_back( midpoint( ( *neighbourRing )[ (i+1+iDiff) % selfRing->size() ],
						  ( *selfRing )[i+1] ) );
		cylVertices_->push_back( midpoint( ( *neighbourRing )[ (i+iDiff) % selfRing->size() ],
						   ( *selfRing )[i] ) );
	}

	for ( unsigned int j = 0; j < selfRing->size()-1; ++j )
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
}
