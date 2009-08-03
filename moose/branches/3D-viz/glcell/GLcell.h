/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/ref_ptr>
#include <osg/ShapeDrawable>

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

	//	static void recvField( const Conn* c, string value );

	static const double EPSILON;

 private:
	string strPath_;
	string fieldValue_;

	vector< Id > renderList_;
	void add2RenderList( Id id );

	vector< osg::ref_ptr< osg::ShapeDrawable> > renderListShapes_;
	void renderRenderList();
};
