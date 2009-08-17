/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/*#include <osg/ref_ptr>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp> */ // karan

#include "GLcellCompartment.h"

class GLcell
{
 public:
	GLcell();

	static void process( const Conn* c, ProcInfo p );
	
	static void processFunc( const Conn* c, ProcInfo p );
        void processFuncLocal( Eref e, ProcInfo info );
	
	static void reinitFunc( const Conn* c, ProcInfo info );
	void reinitFuncLocal( const Conn* c );

	static void setPath( const Conn* c, string strPath );
	void innerSetPath( const string& strPath );
	static string getPath( Eref e );

	static void setClientHost( const Conn* c, string strClientHost );
	void innerSetClientHost( const string& strClientHost );
	static string getClientHost( Eref e );

	static void setClientPort( const Conn* c, string strClientPort );
	void innerSetClientPort( const string& strClientPort );
	static string getClientPort( Eref e );

	//	static void recvField( const Conn* c, string value );

	// static const double EPSILON; // karan
	static const int HEADERLENGTH;

 private:
	string strPath_;
	string strClientHost_;
	string strClientPort_;
	string fieldValue_;

	vector< Id > renderList_;
	void add2RenderList( Id id );

	bool connectionUp_; // connection to running viewer client

	/// networking helper functions
	void* getInAddress( struct sockaddr *sa );
	int getSocket( const char* hostname, const char* service );
	int sendAll( int s, char* buf, int* len );

	/* vector< osg::ref_ptr< osg::ShapeDrawable > > renderListShapes_;
	void renderRenderList();

	osgViewer::Viewer* viewer_; // TODO make this a ref_ptr
	*/ // karan

	vector< GLcellCompartment > renderListGLcellCompartments_;
	
};

