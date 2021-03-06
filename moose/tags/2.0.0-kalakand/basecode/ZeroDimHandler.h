/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZERO_DIM_HANDLER_H
#define _ZERO_DIM_HANDLER_H

/**
 * This class manages the data part of Elements having just one
 * data entry.
 */
class ZeroDimHandler: public DataHandler
{
	public:
		/// This is the generic constructor
		ZeroDimHandler( const DinfoBase* dinfo, 
			const vector< DimInfo >& dims, unsigned short pathDepth,
			bool isGlobal );

		/// Special constructor used in Cinfo::makeCinfoElements
		ZeroDimHandler( const DinfoBase* dinfo, char* data );

		/// This is the copy constructor
		ZeroDimHandler( const ZeroDimHandler* other );

		~ZeroDimHandler();

		////////////////////////////////////////////////////////////
		// Information functions
		////////////////////////////////////////////////////////////

		/// Returns data on specified index
		char* data( DataId index ) const;

		/**
		 * Returns the number of data entries on local node
		 */
		unsigned int localEntries() const;

		bool isDataHere( DataId index ) const;

		bool isAllocated() const {
			return ( data_ != 0 );
		}
		
		unsigned int linearIndex( DataId di ) const {
			return 0;
		}

		/**
		 * Returns vector of array indices present at each level of the
		 * path. Here it is empty.
		 */
		vector< vector< unsigned int > > pathIndices( DataId di ) const;

		/**
		 * Return DataId specified by indices vector. 
		 */
		DataId pathDataId( const vector< vector< unsigned int > >& indices )
			const;
		////////////////////////////////////////////////////////////////
		// load balancing functions
		////////////////////////////////////////////////////////////////
		bool innerNodeBalance( unsigned int size,
			unsigned int myNode, unsigned int numNodes );

		bool execThread( ThreadId thread, DataId di ) const;
		////////////////////////////////////////////////////////////////
		// Process function
		////////////////////////////////////////////////////////////////
		/**
		 * calls process on data, using threading info from the ProcInfo
		 */
		void process( const ProcInfo* p, Element* e, FuncId fid ) const;

		/**
		 * Calls OpFunc f on all data entries, using threading info from 
		 * the Qinfo and the specified argument(s)
		 */
		void forall( const OpFunc* f, Element* e, const Qinfo* q,
			const double* arg, unsigned int argSize, unsigned int numArgs )
			const;

		unsigned int getAllData( vector< char* >& data ) const;

		////////////////////////////////////////////////////////////////
		// Data Reallocation functions
		////////////////////////////////////////////////////////////////

		void globalize( const char* data, unsigned int size );

		void unGlobalize();

		/**
		 * Make a single identity copy, doing appropriate node 
		 * partitioning if toGlobal is false.
		 */
		DataHandler* copy( unsigned short newParentDepth,
			unsigned short copyRootDepth,
			bool toGlobal, unsigned int n ) const;

		DataHandler* copyUsingNewDinfo( const DinfoBase* dinfo) const;

		bool resize( unsigned int dimension, unsigned int size );
		
		void assign( const char* orig, unsigned int numOrig );

		////////////////////////////////////////////////////////////////
		// Iterator functions
		////////////////////////////////////////////////////////////////
		/*

		iterator begin( ThreadId threadNum ) const;

		iterator end( ThreadId threadNum ) const;

		void rolloverIncrement( iterator* i ) const;
		*/


	private:
		char* data_;
		/// Specifies which thread is allowed to call it
		ThreadId myThread_;
};

#endif // _ZERO_DIM_HANDLER_H
