/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _SHARED_FINFO_H
#define _SHARED_FINFO_H

/**
 * Finfo for handling shared messages with arbitrary numbers of
 * srcs and dests.
 * The array of pair< Ftype*, RecvFunc > can indicate MsgSrc or MsgDest
 * in any order, and that order will be preserved in the created
 * Finfo. In this array, MsgSrcs are indicated by the presence of
 * a dummyFunc, and Dests by other funcs.
 * This order is enforced when messages are set up. This means that
 * you cannot send a shared message between the same Finfo on
 * identical objects. Other than that case, the convention is to
 * use the same name for the source and destination Finfo of a shared
 * message, because they are usually a bit of both. Furthermore,
 * it is fine to send the message in either direction.
 */
class SharedFinfo: public Finfo
{
		public:

			/**
			 * Here we construct the list of RecvFuncs for the
			 * SharedFinfo based on a list of either Src or
			 * DestFinfos.
			 */
			SharedFinfo( const string& name, Finfo** finfos,
				 unsigned int nFinfos );

			~SharedFinfo()
			{;}

			bool add(
					Element* e, Element* destElm, const Finfo* destFinfo
			) const;

			bool respondToAdd(
					Element* e, Element* src, const Ftype *srcType,
					unsigned int& srcFuncId, unsigned int& returnFuncId,
					unsigned int& destIndex, unsigned int& numDest
			) const;
			
			unsigned int msg() const {
				return msg_;
			}

			/**
			 * Send a message with the arguments in the string.
			 */
			bool strSet( Element* e, const std::string &s )
					const;
			
			/// strGet doesn't work for SharedFinfo
			bool strGet( const Element* e, std::string &s ) const {
				return 0;
			}

			/// This Finfo does not support recvFuncs.
			RecvFunc recvFunc() const {
					return 0;
			}
			
			/**
			 * The SharedFinfo does handles a message.
			 */
			void countMessages( unsigned int& num );

			const Finfo* match( 
				const Element* e, const ConnTainer* c ) const;

			bool isTransient() const {
					return 0;
			}

			/**
			 * Returns true only if the other finfo is the same type
			 * For the SharedFinfo this means that every function
			 * argument matches. Then on top of this, the function
			 * copies over the slot indices.
			 */
			bool inherit( const Finfo* baseFinfo );

			/**
			 * Returns true if the named MsgSrc is present
			 * on the SharedFinfo, or if the name is the name
			 * of the SharedFinfo itself. Passes back the
			 * Slot in the 'ret' field if found.
			 */
			bool getSlot( const string& name, Slot& ret ) const;

			Finfo* copy() const {
				return new SharedFinfo( *this );
			}

			void addFuncVec( const string& cname );

			/**
			 * Returns the identifier for its FuncVec, which handles
			 * all the destination functions.
			 */
			unsigned int funcId() const {
				return fv_->id();
			}

		private:
			FuncList rfuncs_;
			unsigned int msg_;
			vector < string > names_;
			FuncVec* fv_;
			bool isDest_;
};

#endif // _SHARED_FINFO_H
