/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "CompartmentData.h"
#include "GeometryData.h"

enum MSGTYPE
{
	RESET,
	PROCESS,
	PROCESSSYNC,
	DISCONNECT
};

class GLcell
{
 public:
	GLcell();
	~GLcell();

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
	
	static void setAttributeName( const Conn* c, string strAttributeName );
	void innerSetAttributeName( const string& strAttributeName );
	static string getAttributeName( Eref e );

	static void setChangeThreshold( const Conn* c, double changeThreshold );
	void innerSetChangeThreshold( const double changeThreshold );
	static double getChangeThreshold( Eref e );

	static void setSyncMode( const Conn* c, string syncMode );
	void innerSetSyncMode( const bool syncMode );
	static string getSyncMode( Eref e );
	
	static const int MSGTYPE_HEADERLENGTH;
	static const int MSGSIZE_HEADERLENGTH;
	static const char SYNCMODE_ACKCHAR;

 private:
	string strPath_;
	string strClientHost_;
	string strClientPort_;
	string strAttributeName_;
	bool syncMode_;
	double changeThreshold_; // any change in attribute below this value is not updated visually (in non-sync mode)

	int sockFd_;
	bool isConnectionUp_;

	vector< Id > renderList_;
	GeometryData geometryData_;

	map< unsigned int, double > renderMapAttrsLastTransmitted_;
	map< unsigned int, double > renderMapAttrsTransmitted_;

	void add2RenderList( Id id );
	void findNeighbours( Id id, std::vector< unsigned int>& vecResult );
	void findNeighboursOfType( Id id, const string& messageType, const string& targetType, std::vector< unsigned int >& vecResult );

	/// networking helper functions
	void* getInAddress( struct sockaddr *sa );
	int getSocket( const char* hostname, const char* service );
	int sendAll( int socket, char* buf, int* len );
	int recvAll( int socket, char* buf, int* len);
	
	int receiveAck();
	void handlePick( unsigned int idPicked );
	
	template< class T >
	  void transmit( T& data, MSGTYPE messageType);

	void disconnect();
};

