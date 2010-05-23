/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPARSE_MSG_H
#define _SPARSE_MSG_H

/**
 * This is a parallelized sparse message.
 * It is a general message type optimized for sparse matrix like
 * projection patterns. For each source object[DataId] there can be a
 * target object[DataId].
 * For parallel/multithreaded use, we need to guarantee that all requests
 * to the same target object (and all its synapses) are on the same queue.
 * So it builds up a separate SparseMatrix for each thread.
 *
 * It has a specialized version of exec, to select the appropriate
 * SparseMatrix. It goes through the entire set of incoming events.
 *
 * It has a function to do the node/thread decomposition to generate an
 * equivalent of the original sparse matrix, but using only the appropriate
 * RNG seed.
 *
 * A typical case is from an array of IntFire objects to an array of 
 * Synapses, which are array fields of IntFire objects.
 * The sparse connectivity maps between the source IntFire and target
 * Synapses.
 * The location of the entry in the sparse matrix provides the index of
 * the target IntFire.
 * The data value in the sparse matrix provides the index of the Synapse
 * at that specific connection.
 * This assumes that only one Synapse mediates a given connection between
 * any two IntFire objects.
 *
 * It is optimized for input coming on Element e1, and going to Element e2.
 * If you expect any significant backward data flow, please use 
 * BiSparseMsg.
 * It can be modified after creation to add or remove message entries.
 */
class SparseMsg: public Msg
{
	friend void initMsgManagers(); // for initializing Id.
	public:
		SparseMsg( Element* e1, Element* e2 );
		~SparseMsg();

		void exec( const char* arg, const ProcInfo* p ) const;
		
		unsigned int randomConnect( double probability );

		void loadBalance( unsigned int numThreads );
		void loadUnbalance();

		Id id() const;

		FullId findOtherEnd( FullId end ) const;

		void setMatrix( const SparseMatrix< unsigned int >& m );
		SparseMatrix< unsigned int >& getMatrix();

	private:
		SparseMatrix< unsigned int > matrix_;
		unsigned int numThreads_; // Number of threads to partition
		unsigned int nrows_; // The original size of the matrix.
		static Id id_; // The Element that manages Sparse Msgs.
};

extern void sparseMatrixBalance( 
	unsigned int numThreads, SparseMatrix< unsigned int >& matrix );

class SparseMsgWrapper: public MsgManager
{
	public:
		void setRandomConnectivity( double probability, long seed );
		double getProbability() const;
		void setProbability( double value );

		long getSeed() const;
		void setSeed( long value );

		void setMatrix( const SparseMatrix< unsigned int >& m );

		void setEntry( unsigned int row, unsigned int column, 
			unsigned int value );

		void unsetEntry( unsigned int row, unsigned int column );

		// Still need to implement array field gets.

		unsigned int getNumRows() const;
		unsigned int getNumColumns() const;
		unsigned int getNumEntries() const;
		void clear();
		void transpose();
		void loadBalance( unsigned int numThreads );
		void loadUnbalance();

		static const Cinfo* initCinfo();
	private:
		double p_;
		unsigned long seed_;
};

#endif // _SPARSE_MSG_H
