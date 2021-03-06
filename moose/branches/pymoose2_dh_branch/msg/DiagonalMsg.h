/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _DIAGONAL_MSG_H
#define _DIAGONAL_MSG_H

/**
 * Connects up a series of data entries onto a matching series in a
 * target Element.
 * Inspects each entry, and uses the 'stride_' field to determine
 * which should be its target.
 * \verbatim
 * Suppose we have a stride of +1. Then
 * Src:		1	2	3	4	n
 * Dest:	2	3	4	5	n+1
 *
 * Suppose we have a stride of -2. Then
 * Src:		1	2	3	4	n
 * Dest:	-	-	1	2	n-2
 * \endverbatim
 *
 */
class DiagonalMsg: public Msg
{
	friend void Msg::initMsgManagers(); // for initializing Id.
	public:
		DiagonalMsg( MsgId mid, Element* e1, Element* e2 );
		~DiagonalMsg();

		void exec( const Qinfo* q, const double* arg, FuncId fid ) const;

		Eref firstTgt( const Eref& src ) const;

		/*
		static bool add( Element* e1, const string& srcField, 
			Element* e2, const string& destField, int stride );
			*/

		Id managerId() const;

		ObjId findOtherEnd( ObjId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;

		unsigned int srcToDestPairs(
			vector< DataId >& src, vector< DataId >& dest) const;

		/**
		 * The stride is the increment to the src DataId that gives the dest
		 * DataId. It can be positive or negative, but bounds checking
		 * takes place and it does not wrap around.
		 * This function assigns the stride.
		 */
		void setStride( int stride );

		/**
		 * The stride is the increment to the src DataId that gives the dest
		 * DataId. It can be positive or negative, but bounds checking
		 * takes place and it does not wrap around.
		 * This function reads the stride.
		 */
		int getStride() const;

		/// Setup function for Element-style access to Msg fields.
		static const Cinfo* initCinfo();
	private:
		int stride_; // Increment between targets.
		unsigned int numThreads_;
		unsigned int numNodes_;
		static Id managerId_;
};

#endif // _DIAGONAL_MSG_H
