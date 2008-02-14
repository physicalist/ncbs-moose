/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _MSGSRC_H
#define _MSGSRC_H


/**
 * The MsgSrc class specifies the ranges of conns to be used for
 * messaging, and specifies which functions to be used within the
 * range. In case there are additional ranges with different funcs
 * but the same src, we use the next_ index to point to a location
 * further up in the array, past the predefined src range.
 * Used for the source of simple messages, and for either end of 
 * bidirectional shared messages.
 */
class MsgSrc
{
		public:
			MsgSrc( const FuncVec* fv )
					: fv_( fv ), next_( 0 )
			{;}
			
			MsgSrc( )
					: fv_( 0 ), next_( 0 )
			{;}

			RecvFunc recvFunc( unsigned int func ) const {
					return fl->func( func );
			}

			Conn** nextConn() {
			}

			unsigned int begin() const {
					return begin_;
			}

			unsigned int end() const {
					return end_;
			}

			unsigned int next() const {
					return next_;
			}


			void insertWithin() {
				++end_;
			}

			void insertBefore() {
				++begin_;
				++end_;
			}

			/**
			 * dropConn removes the specified Conn, by rearranging the
			 * indices that bracket the used Conn range.
			 * If there are no Conns left then the function must
			 * be cleared out so that future use of this slot has a
			 * clear slate.
			 */
			void dropConn( unsigned int i ) {
				if ( i < end_ ) {
					--end_;
					if ( i < begin_ )
						--begin_;
				}
				if ( end_ == begin_ )
					recvFunc_ = &dummyFunc;
			}

			void setFunc( RecvFunc rf ) {
					recvFunc_ = rf;
			}

			void setNext( unsigned int i ) {
					next_ = i;
			}

		private:
			vector< Conn* > c_;
			const FuncVec* fv_; // Points to a global FuncList.
			unsigned int next_; // Could use index or ptr here.
};

#endif // _MSGSRC_H

