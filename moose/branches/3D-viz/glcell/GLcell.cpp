/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <iostream>
#include "moose.h"
#include "GLcell.h"
#include "../shell/Shell.h"
#include <osg/ref_ptr>
#include <osgDB/Registry>
#include <osgDB/WriteFile>
#include <osg/Notify>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Quat>

const double GLcell::EPSILON = 1e-9;

const Cinfo* initGLcellCinfo()
{
	static Finfo* processShared[] = 
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
			RFCAST( &GLcell::processFunc ) ),
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
			RFCAST( &GLcell::reinitFunc ) ),
	};
	static Finfo* process = new SharedFinfo( "process", processShared,
		sizeof( processShared ) / sizeof( Finfo* ),
		"shared message to receive Process messages from scheduler objects" );

	/*
	// written to access field values before I was aware of get<class T>(...), the
	// following is now disabled.
	static Finfo* parserShared[] = 
	{
		new SrcFinfo( "get", Ftype2< Id, string >::global(),
			"Getting a field value as a string: send out request:" ),
		new DestFinfo( "recvField", Ftype1< string >::global(),
			RFCAST( &GLcell::recvField ),
			"Getting a field value as a string: receive the value." ),
	};

	static Finfo* parser = new SharedFinfo( "parser", parserShared,
		sizeof( parserShared ) / sizeof( Finfo* ),
		"shared message to talk to the Shell" );
	*/

	static Finfo* GLcellFinfos[] = 
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		new ValueFinfo( "path",
			ValueFtype1< string >::global(),
			GFCAST( &GLcell::getPath ),
			RFCAST( &GLcell::setPath )
				),
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
		//		parser,
		process,
	};
	
	// Schedule molecules for the slower clock, stage 0.
	static SchedInfo schedInfo[] = { { process, 0, 0 } };
	
	static string doc[] = 
	{
		"Name", "GLcell",
		"Author", "Karan Vasudeva, 2009, NCBS",
		"Description", "GLcell: class to drive the 3D cell visualization widget",
	};

	static Cinfo glcellCinfo(
				 doc,
				 sizeof( doc ) / sizeof( string ),
				 initNeutralCinfo(),
				 GLcellFinfos,
				 sizeof( GLcellFinfos ) / sizeof( Finfo * ),
				 ValueFtype1< GLcell >::global(),
				 schedInfo, 1
				 );

	return &glcellCinfo;
}

static const Cinfo* glcellCinfo = initGLcellCinfo();

//static const Slot requestFieldSlot = initGLcellCinfo()->getSlot( "parser.get" );

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

GLcell::GLcell()
	:
	strPath_("")
{
	;
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void GLcell::setPath( const Conn* c, string strPath )
{
	static_cast< GLcell * >( c->data() )->innerSetPath( strPath );
}

void GLcell::innerSetPath( const string& strPath )
{
	strPath_ = strPath;
}

string GLcell::getPath( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->strPath_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void GLcell::processFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->processFuncLocal( c->target(), info );
}

void GLcell::processFuncLocal( Eref e, ProcInfo info )
{
	// Update colors from Vm values per compartment.

	for ( int i = 0; i < renderList_.size(); ++i )
	{
		// process only if element is of type "Compartment":
		if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) ) )
			std::cout << renderList_[i] << " ";
		// TODO will this work in parallel cases? is it permitted to look up cinfo() directly?
	}
	std::cout << std::endl;
}

void GLcell::reinitFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->reinitFuncLocal( c );
}

void GLcell::reinitFuncLocal( const Conn* c )
{
	// Reload model geometry.
	double diameter, length, x0, y0, z0, x, y, z, Vm;

	// strPath_ must be set explicitly.
	if ( !strPath_.empty() )
	{
		// renderList_ holds the flattened tree of elements to render.
		// renderListShapes_ holds a corresponding list of osg::ShapeDrawables.
		renderList_.clear();
		renderListShapes_.clear();
	
		// Start populating renderList_ with the node in strPath_ 
		// and its children, recursively.
		add2RenderList( Shell::path2eid( strPath_, "/", 1 ) );
		
		for ( int i = 0; i < renderList_.size(); ++i )
		{
	
	 	 // written to access field values before I was aware of get<class T>(...), the
		 // following is now disabled.
			/*send2< Id, string >( c->target(), requestFieldSlot, renderList_[i], "dia" );
			  std::cout << "sent " ;
			  if ( fieldValue_.length() > 0 )
			    std::cout << fieldValue_ << std::endl;
			  else
			    std::cout << "but got nothing" << std::endl;*/
			
			if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) )
				&& get< double >( renderList_[i].eref(), "diameter", diameter )
				&& get< double >( renderList_[i].eref(), "length", length )
				&& get< double >( renderList_[i].eref(), "x0", x0 )
				&& get< double >( renderList_[i].eref(), "y0", y0 )
				&& get< double >( renderList_[i].eref(), "z0", z0 )
				&& get< double >( renderList_[i].eref(), "x", x )
				&& get< double >( renderList_[i].eref(), "y", y )
				&& get< double >( renderList_[i].eref(), "z", z )
				&& get< double >( renderList_[i].eref(), "Vm", Vm )
				)
			{
				if (length < GLcell::EPSILON) // i.e., length == 0	
				{ // the compartment is spherical
					osg::ref_ptr< osg::Sphere > sphere = new osg::Sphere( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ), diameter/2 );
					osg::ref_ptr< osg::ShapeDrawable > drawable = new osg::ShapeDrawable( sphere.get() );
					
					renderListShapes_.push_back( drawable.get() );
				}
				else
				{ // the compartment is cylindrical
					osg::ref_ptr< osg::Cylinder > cylinder = new osg::Cylinder( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ), diameter/2, length );

					/// OSG Cylinders are initially oriented along (0,0,1). To orient them
					/// according to readcell's specification of (the line joining) the
					/// centers of their circular faces, they must be rotated around 
					/// an axis given by the cross-product of the initial and final
					/// vectors, by an angle given by the dot-product of the same.
					
					/// Also note that although readcell's conventions for the x,y,z directions
					/// differ from OSG's, (in OSG z is down to up and y is perpendicular 
					/// to the display, pointing inward), OSG's viewer seems to
					/// choose a good viewpoint by default.

					osg::Vec3f initial( 0.0f, 0.0f, 1.0f);
					initial.normalize();

					osg::Vec3f final( x-x0, y-y0, z-z0 );
					final.normalize();

					osg::Quat::value_type angle = acos( initial * final );
					osg::Vec3f axis = initial ^ final;
					axis.normalize();

					cylinder->setRotation( osg::Quat( angle, axis ) ) ;
					osg::ref_ptr< osg::ShapeDrawable > drawable = new osg::ShapeDrawable( cylinder.get() );

					renderListShapes_.push_back( drawable.get() );
				}
			
			}

		}
	}

	// TODO this will be removed
	renderRenderList();
}

/*
// written to access field values before I was aware of get<class T>(...), the
// following is now disabled.
void GLcell::recvField( const Conn* c, string value )
{
	static_cast< GLcell* >( c->data() )->fieldValue_ = value;
}
*/


///////////////////////////////////////////////////
// private function definitions
///////////////////////////////////////////////////

void GLcell::add2RenderList( Id id )
{
	vector< Id > children;
	Id found;

	// Save this node on renderList_, the flattened tree of elements to render.
	renderList_.push_back( id );
	
	// Determine this node's (immediate) children by tracing outgoing "childSrc" messages.
	Conn* i = id()->targets( "childSrc", 0 );
	for ( ; i->good(); i->increment() )
	{
		found = i->target()->id();

		children.push_back( found );
	}

	// If there are any children, call add2RenderList on each of them.
	for ( int j = 0; j < children.size(); ++j )
	{
		add2RenderList( children[j] );
	}
}

void GLcell::renderRenderList()
{
	osg::ref_ptr< osg::Geode > root = new osg::Geode;
	
	for (int i = 0; i < renderListShapes_.size(); ++i)
		root->addDrawable( renderListShapes_[i].get() );

	if ( !root.valid() )
	{
		osg::notify( osg::FATAL ) << "Failed in createSceneGraph()." << std::endl;
		return;
	}

	std::string out( "test.osg" );
	if ( !( osgDB::writeNodeFile( *( root.get() ), out ) ) )
	{
		osg::notify( osg::FATAL ) << "Failed in osgDB::writeNodeFile()." << std::endl;
		return;
	}

	osg::notify( osg::ALWAYS ) << "Success! Use osgviewer to view " << out << std::endl;
}
