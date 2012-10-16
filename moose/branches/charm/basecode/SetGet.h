/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SETGET_H
#define _SETGET_H

// Forward declaration needed for localGet()
template< class T, class A > class GetOpFunc;

#ifdef USE_CHARMPP
#include "ccs-client.h"
#include "../shell/CcsPackUnpack.h"
#include "../basecode/SetGetWrapper.h"
#include "../shell/ShellProxy.h"
#include "pup_stl.h"
#endif

/**
 * Similar to Field< A >::fastGet(), except that an existing Msg is not needed.
 * 
 * Instant-return call for a single value. Bypasses all the queueing stuff.
 * It is hardcoded so type safety will have to be coded in too:
 * the dynamic_cast will catch it only at runtime.
 * 
 * Perhaps analogous localSet(), localLookupGet(), localGetVec(), etc. should
 * also be added.
 * 
 * Also, will be nice to change this to Field< A >::localGet() to make things
 * more uniform.
 */
template< class T, class A >
A localGet( const Eref& er, string field )
{
	const Finfo* finfo = er.element()->cinfo()->findFinfo( "get_" + field );
	assert( finfo );
	
	const DestFinfo* dest = dynamic_cast< const DestFinfo* >( finfo );
	assert( dest );
	
	const OpFunc* op = dest->getOpFunc();
	assert( op );
	
	const GetOpFunc< T, A >* gop =
		dynamic_cast< const GetOpFunc< T, A >* >( op );
	assert( gop );
	
	return gop->reduceOp( er );
}

// XXX for the following operations, we must ensure that moose-core
// isn't in the middle of some other operation when these are issued.
// therefore, we need some buffering mechanism similar to the Shell
// operations

class SetGet
{
#ifdef USE_CHARMPP
  public:
  static const char *strSetHandlerString(){
    static const char *str = "Moose_SetGet_StrSet";
    return str;
  }

  static const char *strGetHandlerString(){
    static const char *str = "Moose_SetGet_StrGet";
    return str;
  }
#endif

	public:
		SetGet( const ObjId& oid )
			: oid_( oid )
		{
			if ( oid_.id.element() == 0 ) {
				cout << "Warning: SetGet: accessing null element: Was object deleted?\n";
			}
		}

		virtual ~SetGet()
		{;}

		/**
		 * Utility function to check that the target field matches this
		 * source type, to look up and pass back the fid, and to return
		 * the number of targetEntries.
		 * Tgt is passed in as the destination ObjId. May be changed inside,
		 * if the function determines that it should be directed to a 
		 * child Element acting as a Value.
		 * Checks arg # and types for a 'set' call. Can be zero to 3 args.
		 * Returns # of tgts if good. This is 0 if bad. 
		 * Passes back found fid.
		 */
		const OpFunc* checkSet( 
			const string& field, ObjId& tgt, FuncId& fid ) const;

//////////////////////////////////////////////////////////////////////
		/**
		 * Blocking 'get' call, returning into a string.
		 * There is a matching 'get<T> call, returning appropriate type.
		 */
		static bool strGet( const ObjId& tgt, const string& field, string& ret );
#ifdef USE_CHARMPP
                static void strGet_handler(char *msg);
		static bool strGet_ccs( const ObjId& tgt, const string& field, string& ret );
#endif

		/**
		 * Blocking 'set' call, using automatic string conversion
		 * There is a matching blocking set call with typed arguments.
		 */
		static bool strSet( const ObjId& dest, const string& field, const string& val );
#ifdef USE_CHARMPP
                static void strSet_handler(char *msg);
		static bool strSet_ccs( const ObjId& dest, const string& field, const string& val );
#endif

		/// Sends out request for data, and awaits its return.
#ifndef USE_CHARMPP
		static const vector< double* >* dispatchGet( 
			const ObjId& tgt, FuncId tgtFid, 
			const double* arg, unsigned int size );
#endif

		static Qinfo qi_;

#ifdef USE_CHARMPP
                // XXX must be initialized appropriately
                // XXX ensure that these operations are only called from 
                // head node, by a single thread.
                static CcsServer ccsServer_;

        struct Args {
          ObjId dest_;
          string field_;

          Args(ObjId dest, string field) : 
            dest_(dest),
            field_(field)
          {}

          Args() {}

          void pup(PUP::er &p){
            p | dest_;
            p | field_;
          }
        };
#endif

	private:
		ObjId oid_;
};

class SetGet0: public SetGet
{
  
  private:
#ifdef USE_CHARMPP
  static const char *setHandlerString(){
    const char *str = "SetGet0_setHandlerString";
    return str;
  }
#endif


	public:
		SetGet0( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */
#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field ){
                        unsigned int size;
                        SetGet::Args args(dest, field);
                        char *msg = CcsPackUnpack<SetGet::Args>::pack(args, size);
                        CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet0::setHandlerString(), size, msg);
                        delete[] msg;

                        bool ret;
                        while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                        return false;
                        // return ret;
                }

                static void set_handler(char *msg){
                  SetGet::Args args;
                  CcsPackUnpack<SetGet::Args>::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
                static bool set(const ObjId &dest, string field)
                {
			SetGet0 sg( dest );
			FuncId fid;
			ObjId tgt( dest );

			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				func->op( tgt.eref(), &qi_, 0 );
				/*
				mpiSend( fid, tgt, 0, 0 );
				*/
			}
			return 0;
		}

		/**
		 * Blocking call using string conversion
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			return set( dest, field );
		}
};

template< class A > class SetGet1: public SetGet
{

#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;
  static char *setVecHandlerString_;

  public:
  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }

  static void setVecHandlerString(const char *str){
    setVecHandlerString_ = str;
  }

  static const char *setVecHandlerString(){
    return setVecHandlerString_;
  }
#endif

  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    vector<A> values_;

    Args(const ObjId &objId, string field, const A &value) : 
      SetGet::Args(objId, field)
    {
      values_.push_back(value);
    }

    Args(const ObjId &objId, string field, const vector<A> &values) : 
      SetGet::Args(objId, field),
      values_(values)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | values_; 
    }
  };
#endif

	public:
		SetGet1( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */

#ifdef USE_CHARMPP
                static bool set_ccs(const ObjId& dest, const string& field, A arg){
                  unsigned int size;
                  SetGet1< A >::Args wrapper(dest, field, arg);
                  char *msg = CcsPackUnpack< SetGet1< A >::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet1< A >::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  // original version always returned 'false'
                  return false;
                  // return ret;
                }

                static void set_handler(char *msg){
                  SetGet1< A >::Args args;
                  CcsPackUnpack< SetGet1< A >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.values_[0]);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
		static bool set( const ObjId& dest, const string& field, const A &arg )
		{
			SetGet1< A > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A > conv( arg );
				func->op( tgt.eref(), &qi_, conv.ptr() );
				// mpiSend( fid, tgt, conv.ptr, conv.size() );
				return 1;
			}
			return 0;
		}

		/**
		 * I would like to provide a vector set operation. It should
		 * work in three modes: Set all targets to the same value,
		 * set targets one by one to a vector of values, and set targets
		 * one by one to randomly generated values within a range. All
		 * of these can best be collapsed into the vector assignment 
		 * operation.
		 * This variant requires that all vector entries have the same
		 * size. Strings won't work.
		 *
		 * The sequence of data in the buffer is:
		 * first few locations: ObjFid
		 * Remaining locations: Args.
		 */

                public:
#ifdef USE_CHARMPP
                static bool setVec_ccs(const Id dest, const string &field, const vector<A> &arg){
                  unsigned int size;
                  SetGet1< A >::Args wrapper(ObjId(dest), field, arg);
                  char *msg = CcsPackUnpack< SetGet1< A >::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet1::setVecHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  // original version always returned 'false'
                  return false;
                  // return ret;
                }

                static void setVec_handler(char *msg){
                  SetGet1< A >::Args args;
                  CcsPackUnpack< SetGet1< A >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = setVec(args.dest_.id, args.field_, args.values_);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
		static bool setVec( Id destId, const string& field, const vector< A >& arg )
		{
			if ( arg.size() == 0 ) return 0;

			ObjId tgt( destId, 0 );
			SetGet1< A > sg( tgt );
			FuncId fid;

			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A > conv( arg[0] );
				unsigned int entrySize = conv.size();
				tgt.dataId = DataId::any;
				double* data = new double[ entrySize * arg.size() ];
				double* ptr = data;

				for ( unsigned int i = 0; i < arg.size(); ++i ) {
					Conv< A > conv( arg[i] );
					assert( conv.size() == entrySize );
					memcpy( ptr, conv.ptr(), entrySize * sizeof( double ) );
					ptr += entrySize;
				}
				Element* elm = tgt.id.element();
				elm->dataHandler()->forall( func, elm, &qi_, 
					data, entrySize, arg.size() );

				delete[] data;

				return 1;
			}
			return 0;
		}

                public:
		/**
		 * Sets all target array values to the single value
		 */
		static bool setRepeat( Id destId, const string& field, 
			const A& arg )
		{
			vector< A >temp ( 1, arg );
			return setVec( destId, field, temp );
		}

#ifdef USE_CHARMPP
		static bool setRepeat_ccs( Id destId, const string& field, 
			const A& arg )
		{
			vector< A >temp ( 1, arg );
			return setVec_ccs( destId, field, temp );
		}

#endif

		/**
		 * Blocking call using string conversion
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A arg;
			Conv< A >::str2val( arg, val );
			return set( dest, field, arg );
		}
};

template< class A > class Field: public SetGet1< A >
{

#ifdef USE_CHARMPP
  private: 

  static char *getHandlerString_;
  static char *getVecHandlerString_;

  public:

  static void getHandlerString(const char *str){
    getHandlerString_ = str;
  }

  static const char *getHandlerString(){
    return getHandlerString_;
  }

  static void getVecHandlerString(const char *str){
    getVecHandlerString_ = str;
  }

  static const char *getVecHandlerString(){
    return getVecHandlerString_;
  }
#endif


	public:
		Field( const ObjId& dest )
			: SetGet1< A >( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */
		static bool set( const ObjId& dest, const string& field, A arg )
		{
			string temp = "set_" + field;
			return SetGet1< A >::set( dest, temp, arg );
		}

#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, A arg )
		{
			string temp = "set_" + field;
			return SetGet1< A >::set_ccs( dest, temp, arg );
		}

#endif

		static bool setVec( Id destId, const string& field, 
			const vector< A >& arg )
		{
			string temp = "set_" + field;
			return SetGet1< A >::setVec( destId, temp, arg );
		}

		static bool setRepeat( Id destId, const string& field, 
			A arg )
		{
			string temp = "set_" + field;
			return SetGet1< A >::setRepeat( destId, temp, arg );
		}

		/**
		 * Blocking call using string conversion
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A arg;
			// Do NOT add 'set_' to the field name, as the 'set' func
			// does it anyway.
			Conv< A >::str2val( arg, val );
			return set( dest, field, arg );
		}

	//////////////////////////////////////////////////////////////////
#ifdef USE_CHARMPP
		static A get_ccs( const ObjId& dest, const string& field){
                  unsigned int size;
                  SetGet::Args wrapper(dest, field);
                  char *msg = CcsPackUnpack< SetGet::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, Field< A >::getHandlerString(), size, msg);
                  delete[] msg;

                  int msgSize;
                  // don't know how much data to receive,
                  // so receive the whole msg first and 
                  // unpack the data within it
                  while(CcsRecvResponseMsg(&SetGet::ccsServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0);
                  // type A might need to be unwrapper
                  SetGet1Wrapper< A > ret;
                  CcsPackUnpack< SetGet1Wrapper< A > >::unpack(msg, ret);
                  // msg will point to a malloc()ed buffer after CcsRecvResponseMsg
                  free(msg);
                  CkAssert(ret.hasData_);
                  return ret.a1_;
                }

                static void get_handler(char *msg){
                  SetGet::Args args;
                  CcsPackUnpack<SetGet::Args>::unpack(msg, args);
                  CmiFree(msg);

                  A ret;
                  bool success = get(args.dest_, args.field_, ret); 
                  // A might have members that need to be wrapper
                  unsigned int size;
                  SetGet1Wrapper< A > wrapper(ret, success);
                  msg = CcsPackUnpack< SetGet1Wrapper< A > >::pack(wrapper, size);

                  CcsSendReply(size, msg);
                  delete[] msg;
                }

#endif
                static A get( const ObjId &dest, const string &field){
                  A ret;
                  get(dest, field, ret);
                  return ret;
                }

		static bool get( const ObjId& dest, const string& field, A &ret)
		{ 
			Field< A > sg( dest );
			ObjId tgt( dest );
			FuncId fid;
			string fullFieldName = "get_" + field;
			if ( const OpFunc* func = 
				sg.checkSet( fullFieldName, tgt, fid ) ) {
				/// Do something else if off-node.
				const GetOpFuncBase< A >* gof = 
					dynamic_cast< const GetOpFuncBase< A >* >( func );
				if ( gof ){
					ret = gof->reduceOp( tgt.eref() );
                                        return true; 
                                }
			}
			cout << "Warning: Field::Get conversion error for " << dest.id.path() <<
				endl;

			ret = A();
                        return false;
		}

		/**
		 * Blocking call that returns a vector of values
		 * Note that the vector returned by innerGet is sparse due to
		 * the way the indexing is done. We assume
		 * all the entries have come in, and discard the holes.
		 */
#ifdef USE_CHARMPP
		static void getVec_ccs( Id dest, const string& field, vector< A >& vec){
                  unsigned int size;
                  SetGet::Args wrapper(ObjId(dest), field);
                  char *msg = CcsPackUnpack< SetGet::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, Field< A >::getVecHandlerString(), size, msg);
                  delete[] msg;

                  int replySize;
                  while(CcsRecvResponseMsg(&SetGet::ccsServer_, &replySize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0); 
                  SetGet1Wrapper< vector<A> > ret;
                  CcsPackUnpack< SetGet1Wrapper< vector< A > > >::unpack(msg, ret);
                  free(msg);

                  vec = ret.a1_;
                }

                static void getVec_handler(char *msg){
                  SetGet::Args args; 
                  CcsPackUnpack<SetGet::Args>::unpack(msg, args);
                  CmiFree(msg);

                  vector< A > ret;
                  bool success = getVec(args.dest_.id, args.field_, ret);

                  // A might have members that need to be wrapper
                  unsigned int size;
                  SetGet1Wrapper< vector< A > > wrapper(ret, success);
                  msg = CcsPackUnpack< SetGet1Wrapper< vector< A > > >::pack(wrapper, size);

                  CcsSendReply(size, msg);
                  delete[] msg;
                }

#endif
		static bool getVec( Id dest, const string& field, vector< A >& vec)
		{
			vec.resize( 0 );
			Field< A > sg( dest );
			ObjId tgt( dest );
			FuncId fid;
			string fullFieldName = "get_" + field;
			if ( const OpFunc* func = 
				sg.checkSet( fullFieldName, tgt, fid ) )
			{
				const GetOpFuncBase< A >* gof = 
					dynamic_cast< const GetOpFuncBase< A >* >( func );
				if ( gof ) {
					Element* elm = dest.element();
					DataHandler* dh = elm->dataHandler();
					// This will need some serious MPI work.
					unsigned int fieldMask = dh->fieldMask();
					unsigned int size = dh->totalEntries();
					vec.resize( size );
					if ( fieldMask != 0 ) {
						assert( dh->numDimensions() > 0 );
						unsigned int maxNumEntries = dh->sizeOfDim(
							dh->numDimensions() - 1 );
						unsigned int numObj = size / maxNumEntries;
						for ( unsigned int i = 0; i < numObj; ++i ) {
							unsigned int numEntries = dh->getFieldArraySize( i );
							for ( unsigned int j = 0; j < numEntries; ++j ) {
								unsigned int k = j + i * ( fieldMask + 1 );
								DataId di( k );
								k = j + i * maxNumEntries;
								vec[ k ] = gof->reduceOp( Eref( elm, di ));
							}
						}
					} else {
						for ( unsigned int i = 0; i < size; ++i ) {
							Eref e( elm, i );
							vec[i] = gof->reduceOp( e );
						}
					}
					return true;
				}
			}
			cout << "Warning: Field::getVec conversion error for " <<
				dest.path() << endl;
                        return false;
		}
		

		/**
		 * Instant-return call for a single value along an existing Msg.
		 * Bypasses all the queueing stuff.
		 * It is hardcoded so type safety will have to be coded in too:
		 * the dynamic_cast will catch it only at runtime.
		 */
		static A fastGet( const Eref& src, MsgId mid, FuncId fid )
		{
			const Msg* m = Msg::getMsg( mid );
			if ( m ) {
				Eref tgt = m->firstTgt( src );
				if ( tgt.element() ) {
					const GetOpFuncBase< A >* gof = 
						dynamic_cast< const GetOpFuncBase< A >* >(
						tgt.element()->cinfo()->getOpFunc( fid ) );
					if ( gof ) {
						return gof->reduceOp( tgt );
					}
				}
			}
			cout << "Warning: fastGet failed for " << src.id().path() <<
				endl;
			return A();
		}

		/**
		 * Blocking call for finding a value and returning in a
		 * string.
		 */
		static bool innerStrGet( const ObjId& dest, const string& field, 
			string& str )
		{
			Conv< A >::val2str( str, get( dest, field ) );
			return 1;
		}
};

/**
 * SetGet2 handles 2-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2 > class SetGet2: public SetGet
{

#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;
  static char *setVecHandlerString_;

  public:

  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }

  static void setVecHandlerString(const char *str){
    setVecHandlerString_ = str;
  }

  static const char *setVecHandlerString(){
    return setVecHandlerString_;
  }
#endif


  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    vector< A1 > a1_;
    vector< A2 > a2_;

    Args(const ObjId &objId, string field, A1 a1, A2 a2) : 
      SetGet::Args(objId, field) 
    {
      a1_.push_back(a1);
      a2_.push_back(a2);
    }

    Args(const ObjId &objId, string field, const vector< A1 > &a1, const vector< A2 > &a2) : 
      SetGet::Args(objId, field),
      a1_(a1),
      a2_(a2)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | a1_;
      p | a2_;
    }
  };
#endif
	public:
		SetGet2( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */
#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, A1 arg1, A2 arg2 ){
                  unsigned int size;
                  SetGet2::Args wrapper(dest, field, arg1, arg2);
                  char *msg = CcsPackUnpack< SetGet2::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet2::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void set_handler(char *msg){
                  SetGet2::Args args;
                  CcsPackUnpack< SetGet2::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.a1_[0], args.a2_[0]);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
		static bool set( const ObjId& dest, const string& field, 
			A1 arg1, A2 arg2 )
		{
			SetGet2< A1, A2 > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1 );
				Conv< A2 > conv2( arg2 );
				unsigned int totSize = conv1.size() + conv2.size();
				double *temp = new double[ totSize ];
				conv1.val2buf( temp );
				conv2.val2buf( temp + conv1.size() );
				func->op( tgt.eref(), &qi_, temp );
				delete[] temp;
				/*
				Qinfo::addDirectToQ( 
					ObjId(), tgt, 0, fid, 
					conv1.ptr(), conv1.size(),
					conv2.ptr(), conv2.size() );
				Qinfo::waitProcCycles( 1 );
				*/
				return true;
			}
			return false;
		}

		/**
		 * Assign a vector of targets, using matching vectors of arguments
		 * arg1 and arg2. Specifically, index i on the target receives
		 * arguments arg1[i], arg2[i].
		 * Note that there is no requirement for the size of the 
		 * argument vectors to be equal to the size of the target array
		 * of objects. If there are fewer arguments then the index cycles
		 * back, so as to tile the target array with as many arguments as
		 * we have.
		 * Need to clean up to handle string arguments later.
		 */
#ifdef USE_CHARMPP
		static bool setVec_ccs( Id destId, const string& field, const vector< A1 >& arg1, const vector< A2 >& arg2 ){
                  unsigned int size;
                  Args wrapper(ObjId(destId), field, arg1, arg2);
                  char *msg = CcsPackUnpack< Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet2::setVecHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void setVec_handler(char *msg){
                  SetGet2::Args args;
                  CcsPackUnpack< SetGet2::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = setVec(args.dest_, args.field_, args.a1_, args.a2_);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
		static bool setVec( Id destId, const string& field, 
			const vector< A1 >& arg1, const vector< A2 >& arg2 )
		{
			if ( arg1.size() != arg2.size() || arg1.size() == 0 )
				return 0;
			ObjId tgt( destId, 0 );
			SetGet2< A1, A2 > sg( tgt );
			FuncId fid;
			// Need to do something similar for MPI.
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1[0] );
				Conv< A2 > conv2( arg2[0] );
				unsigned int entrySize = conv1.size() + conv2.size();
				tgt.dataId = DataId::any;
				double* data = new double[ entrySize * arg1.size() ];
				double* ptr = data;

				for ( unsigned int i = 0; i < arg1.size(); ++i ) {
					Conv< A1 > conv1( arg1[i] );
					Conv< A2 > conv2( arg2[i] );
					memcpy( ptr, conv1.ptr(), conv1.size() * sizeof( double ) );
					ptr += conv1.size();
					memcpy( ptr, conv2.ptr(), conv2.size() * sizeof( double ) );
					ptr += conv2.size();
				}
				Element* elm = tgt.id.element();
				elm->dataHandler()->forall( func, elm, &qi_, 
					data, entrySize, arg1.size() );
				delete[] data;
				return 1;
			}
			return 0;
		}

		/**
		 * This setVec takes a specific object entry, presumably one with
		 * an array of values within it. The it goes through each specified
		 * index and assigns the corresponding argument.
		 * This is a brute-force assignment.
		 */
		static bool setVec( ObjId dest, const string& field, 
			const vector< A1 >& arg1, const vector< A2 >& arg2 )
		{
			unsigned int max = arg1.size();
			if ( max > arg2.size() ) 
				max = arg2.size();
			bool ret = 1;
			for ( unsigned int i = 0; i < max; ++i )
				ret &= 
					SetGet2< A1, A2 >::set( dest, field, arg1[i], arg2[i] );
			return ret;
		}

                // XXX give corresponding _ccs functions if required

		/**
		 * Blocking call using string conversion.
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A1 arg1;
			A2 arg2;
			string::size_type pos = val.find_first_of( "," );
			Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
			Conv< A2 >::str2val( arg2, val.substr( pos + 1 ) );
			return set( dest, field, arg1, arg2 );
		}

	//////////////////////////////////////////////////////////////////
	//  The 'Get' calls for 2 args are currently undefined.
	//////////////////////////////////////////////////////////////////
	
		/**
		 * Terminating call using string conversion
		 */
		string harvestStrGet() const
		{ 
			return "";
		}
};

/**
 * LookupField handles fields that have an index arguments. Examples include
 * arrays and maps.
 * The first argument in the 'Set' is the index, the second the value.
 * The first and only argument in the 'get' is the index.
 * Here A is the type of the value, and L the lookup index.
 * 
 */
template< class L, class A > class LookupField: public SetGet2< L, A >
{
#ifdef USE_CHARMPP
  private:
  static const char *getHandlerString_;

  public:
  static void getHandlerString(const char *str){
    getHandlerString_ = str;
  }

  static const char *getHandlerString(){
    return getHandlerString_;
  }

#endif

	public:
		LookupField( const ObjId& dest )
			: SetGet2< L, A >( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call. Identical to SetGet2::set.
		 */
		static bool set( const ObjId& dest, const string& field, 
			L index, A arg )
		{
			string temp = "set_" + field;
			return SetGet2< L, A >::set( dest, temp, index, arg );
		}

		/** 
		 * This setVec assigns goes through each object entry in the
		 * destId, and assigns the corresponding index and argument to it.
		 */
		static bool setVec( Id destId, const string& field, 
			const vector< L >& index, const vector< A >& arg )
		{
			string temp = "set_" + field;
			return SetGet2< L, A >::setVec( destId, temp, index, arg );
		}

		/**
		 * This setVec takes a specific object entry, presumably one with
		 * an array of values within it. The it goes through each specified
		 * index and assigns the corresponding argument.
		 * This is a brute-force assignment.
		 */
		static bool setVec( ObjId dest, const string& field, 
			const vector< L >& index, const vector< A >& arg )
		{
			string temp = "set_" + field;
			return SetGet2< L, A >::setVec( dest, temp, index, arg );
		}

		/**
		 * Faking setRepeat too. Just plugs into setVec.
		 */
		static bool setRepeat( Id destId, const string& field, 
			const vector< L >& index, A arg )
		{
			vector< A > avec( index.size(), arg );
			return setVec( destId, field, index, avec );
		}

		/**
		 * Blocking call using string conversion
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& indexStr, const string& val )
		{
			L index;
			Conv< L >::str2val( index, indexStr );

			A arg;
			// Do NOT add 'set_' to the field name, as the 'set' func
			// does it anyway.
			Conv< A >::str2val( arg, val );
			return set( dest, field, index, arg );
		}

	//////////////////////////////////////////////////////////////////
		static const vector< double* >* innerGet( 
			const ObjId& dest, const string& field, L index )
		{ 
			LookupField< L, A > sg( dest );
			ObjId tgt( dest );
			FuncId fid;

			cerr << "Error: LookupField::innerGet not yet working\n";
			assert( 0 );

			string fullFieldName = "get_" + field;
			if ( const OpFunc* func = 
				sg.checkSet( fullFieldName, tgt, fid ) ) 
			{
				const GetOpFuncBase< A >* gof = dynamic_cast< const GetOpFuncBase< A >* >( func );
				const vector< double* >* ret = 0;
				if ( gof ) {
					// Iterate over the entries here.
					;
				}
/*
				FuncId retFuncId = receiveGet()->getFid();
				Conv< FuncId > conv1( retFuncId );
				Conv< L > conv2 ( index );
				double* temp = new double[ conv1.size() + conv2.size() ];
				conv1.val2buf( temp );
				conv2.val2buf( temp + conv1.size() );
				const vector< double* >* ret = 
					SetGet::dispatchGet( tgt, fid, temp, 
						conv1.size() + conv2.size() );
				delete[] temp;

				*/
				return ret;
			}
			return 0;
		}

		/**
		 * Gets a value on a specific object, looking it up using the
		 * provided index.
		 */
#ifdef USE_CHARMPP
                static A get_internal(const ObjId& dest, const string& field, L index){
                  unsigned int size;
                  typename SetGet1< L >::Args wrapper(dest, field, index);
                  char *msg = CcsPackUnpack< typename SetGet1< L >::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, LookupField< L, A >::getHandlerString(), size, msg);
                  delete[] msg;

                  int msgSize;
                  while(CcsRecvResponseMsg(&SetGet::ccsServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0);
                  // types L, A might need to be unwrapper
                  SetGet1Wrapper< A > ret;
                  CcsPackUnpack< SetGet1Wrapper< A > >::unpack(msg, ret);
                  // msg will point to a malloc()ed buffer after CcsRecvResponseMsg
                  free(msg);
                  return ret.a1_;
                }

                static void get_handler(char *msg){
                  typename SetGet1< A >::Args args;
                  CcsPackUnpack< typename SetGet1< A >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  A ret;
                  bool success = get(args.dest_, args.field_, args.values_[0], ret); 
                  // A might have members that need to be packed
                  unsigned int size;
                  SetGet1Wrapper< A > wrapper(ret, success);
                  msg = CcsPackUnpack< SetGet1Wrapper< A > >::pack(wrapper, size);

                  CcsSendReply(size, msg);
                  delete[] msg;
                }

#endif
		static A get( const ObjId& dest, const string& field, L index){
                  A ret;
                  get(dest, field, index, ret); 
                  return ret;
                }

		static bool get( const ObjId& dest, const string& field, L index, A &ret)
		{ 
			LookupField< L, A > sg( dest );
			ObjId tgt( dest );
			FuncId fid;
			string fullFieldName = "get_" + field;
			if ( const OpFunc* func = 
				sg.checkSet( fullFieldName, tgt, fid ) ) {
				const LookupGetOpFuncBase< L, A >* gof = 
				dynamic_cast< const LookupGetOpFuncBase< L, A >* >( func );
				if ( gof ){
					ret = gof->reduceOp( tgt.eref(), index );
                                        return true;
                                }
			}
			cout << "Warning: LookupField::Get conversion error for " << 
				dest.id.path() << endl;
                        ret = A();
			return false;
		}

		/**
		 * Blocking call that returns a vector of values in vec.
		 * This variant goes through each target object entry on dest,
		 * and passes in the same lookup index to each one. The results
		 * are put together in the vector vec.
		 * As the vector returned from the innerGet command is sparse but
		 * ordered, we compact it to build up the return vector.
		 */
		static void getVec( Id dest, const string& field, 
			vector< L >& index, vector< A >& vec )
		{
			ObjId tgt( dest, DataId::any );
			const vector< double* >* ret = 
				innerGet( tgt, field, index, ret );
			vec.resize( 0 );
			if ( ret ) {
				vec.resize( ret->size() );
				for ( unsigned int i = 0; i < ret->size(); ++i ) {
					Conv< A > conv( (*ret)[i] );
					vec[i] = *conv;
					// vec.push_back( *conv );
				}
			}
		}

		/**
		 * Blocking virtual call for finding a value and returning in a
		 * string.
		 */
		static bool innerStrGet( const ObjId& dest, const string& field, 
			const string& indexStr, string& str )
		{
			L index;
			Conv< L >::str2val( index, indexStr );

			A ret = get( dest, field, index );
			Conv<A>::val2str( str, ret );
			return 1;
		}
};

/**
 * SetGet3 handles 3-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3 > class SetGet3: public SetGet
{
#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;

  public:
  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }
#endif

  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;

    Args(ObjId dest, string field, A1 a1, A2 a2, A3 a3) :
      SetGet::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
    }
  };
#endif


	public:
		SetGet3( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */

#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, 
			         A1 arg1, A2 arg2, A3 arg3 ){
                  unsigned int size;
                  SetGet3::Args wrapper(dest, field, arg1, arg2, arg3);
                  char *msg = CcsPackUnpack< SetGet3::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet3::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void set_handler(char *msg){
                  SetGet3< A1, A2, A3 >::Args args;
                  CcsPackUnpack< SetGet3< A1, A2, A3 >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.a1_, args.a2_, args.a3_);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif
		static bool set( const ObjId& dest, const string& field, 
			A1 arg1, A2 arg2, A3 arg3 )
		{
			SetGet3< A1, A2, A3 > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1 );
				Conv< A2 > conv2( arg2 );
				Conv< A3 > conv3( arg3 );
				unsigned int totSize = 
					conv1.size() + conv2.size() + conv3.size();
				double *temp = new double[ totSize ];
				conv1.val2buf( temp );
				conv2.val2buf( temp + conv1.size() );
				conv3.val2buf( temp + conv1.size() + conv2.size() );

				func->op( tgt.eref(), &qi_, temp );
				delete[] temp;
				return 1;
			}
			return 0;
		}

		/**
		 * Blocking call using string conversion.
		 * As yet we don't have 2 arg conversion from a single string.
		 * So this is a dummy
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A1 arg1;
			A2 arg2;
			A3 arg3;
			string::size_type pos = val.find_first_of( "," );
			Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
			string temp = val.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A2 >::str2val( arg2, temp.substr( 0,pos ) );
			Conv< A3 >::str2val( arg3, temp.substr( pos + 1 ) );
			return set( dest, field, arg1, arg2, arg3 );
		}

		/**
		 * Terminating call using string conversion
		 */
		string harvestStrGet() const
		{ 
			return "";
		}
};

/**
 * SetGet4 handles 4-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4 > class SetGet4: public SetGet
{

#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;

  public:
  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }
#endif


  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;

    Args(ObjId dest, string field, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) :
      SetGet::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
    }
  };
#endif



	public:
		SetGet4( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */

#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, 
			         A1 arg1, A2 arg2, A3 arg3, A4 arg4){
                  unsigned int size;
                  SetGet4::Args wrapper(dest, field, arg1, arg2, arg3, arg4);
                  char *msg = CcsPackUnpack< SetGet4::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet4::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void set_handler(char *msg){
                  SetGet4< A1, A2, A3, A4 >::Args args;
                  CcsPackUnpack< SetGet4< A1, A2, A3, A4 >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.a1_, args.a2_, args.a3_, args.a4_);
                  CcsSendReply(sizeof(bool), &ret);
                }


#endif
		static bool set( const ObjId& dest, const string& field, 
			A1 arg1, A2 arg2, A3 arg3, A4 arg4 )
		{
			SetGet4< A1, A2, A3, A4 > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1 );
				Conv< A2 > conv2( arg2 );
				Conv< A3 > conv3( arg3 );
				Conv< A4 > conv4( arg4 );
				unsigned int totSize = 
					conv1.size() + conv2.size() + 
					conv3.size() + conv4.size();
				double* temp = new double[ totSize ];
				double* ptr = temp;
				conv1.val2buf( ptr ); ptr += conv1.size();
				conv2.val2buf( ptr ); ptr += conv2.size();
				conv3.val2buf( ptr ); ptr += conv3.size();
				conv4.val2buf( ptr );
				func->op( tgt.eref(), &qi_, temp );

				delete[] temp;
				return 1;
			}
			return 0;
		}

		/**
		 * Blocking call using string conversion.
		 * As yet we don't have 2 arg conversion from a single string.
		 * So this is a dummy
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A1 arg1;
			A2 arg2;
			A3 arg3;
			A4 arg4;
			string::size_type pos = val.find_first_of( "," );
			Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
			string temp = val.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
			Conv< A4 >::str2val( arg4, temp.substr( pos + 1 ) );
			return set( dest, field, arg1, arg2, arg3, arg4 );
		}

	//////////////////////////////////////////////////////////////////
	//  The 'Get' calls for 2 args are currently undefined.
	//////////////////////////////////////////////////////////////////
	
		/**
		 * Terminating call using string conversion
		 */
		string harvestStrGet() const
		{ 
			return "";
		}
};

/**
 * SetGet5 handles 5-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4, class A5 > class SetGet5:
	public SetGet
{
#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;

  public:
  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }
#endif

  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;

    Args(ObjId dest, string field, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) :
      SetGet::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4),
      a5_(a5)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
      p | a5_;
    }
  };
#endif



	public:
		SetGet5( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */
#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, 
			         A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5){
                  unsigned int size;
                  SetGet5::Args wrapper(dest, field, arg1, arg2, arg3, arg4, arg5);
                  char *msg = CcsPackUnpack< SetGet5::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet5::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void set_handler(char *msg){
                  SetGet5< A1, A2, A3, A4, A5 >::Args args;
                  CcsPackUnpack< SetGet5< A1, A2, A3, A4, A5 >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_);
                  CcsSendReply(sizeof(bool), &ret);
                }


#endif

		static bool set( const ObjId& dest, const string& field, 
			                  A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5 )
		{
			SetGet5< A1, A2, A3, A4, A5 > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1 );
				Conv< A2 > conv2( arg2 );
				Conv< A3 > conv3( arg3 );
				Conv< A4 > conv4( arg4 );
				Conv< A5 > conv5( arg5 );

				unsigned int totSize = 
					conv1.size() + conv2.size() + 
					conv3.size() + conv4.size() + conv5.size();
				double* temp = new double[ totSize ];
				double* ptr = temp;
				conv1.val2buf( ptr ); ptr += conv1.size();
				conv2.val2buf( ptr ); ptr += conv2.size();
				conv3.val2buf( ptr ); ptr += conv3.size();
				conv4.val2buf( ptr ); ptr += conv4.size();
				conv5.val2buf( ptr );
				func->op( tgt.eref(), &qi_, temp );

				delete[] temp;
				return 1;
			}
			return 0;
		}

		/**
		 * Blocking call using string conversion.
		 * As yet we don't have 2 arg conversion from a single string.
		 * So this is a dummy
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A1 arg1;
			A2 arg2;
			A3 arg3;
			A4 arg4;
			A5 arg5;
			string::size_type pos = val.find_first_of( "," );
			Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
			string temp = val.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A4 >::str2val( arg4, temp.substr( 0, pos ) );
			Conv< A5 >::str2val( arg5, temp.substr( pos + 1 ) );
			return set( dest, field, arg1, arg2, arg3, arg4, arg5 );
		}
	//////////////////////////////////////////////////////////////////
	//  The 'Get' calls for 2 args are currently undefined.
	//////////////////////////////////////////////////////////////////
	
		/**
		 * Terminating call using string conversion
		 */
		string harvestStrGet() const
		{ 
			return "";
		}
};

/**
 * SetGet6 handles 6-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4, class A5, class A6 > class SetGet6:
	public SetGet
{
#ifdef USE_CHARMPP
  private:
  static char *setHandlerString_;

  public:
  static void setHandlerString(const char *str){
    setHandlerString_ = str;
  }

  static const char *setHandlerString(){
    return setHandlerString_;
  }
#endif

  public:
#ifdef USE_CHARMPP
  struct Args : public SetGet::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;

    Args(ObjId dest, string field, 
         const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6) :
      SetGet::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4),
      a5_(a5),
      a6_(a6)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGet::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
      p | a5_;
      p | a6_;
    }
  };
#endif



	public:
		SetGet6( const ObjId& dest )
			: SetGet( dest )
		{;}

		/**
		 * Blocking, typed 'Set' call
		 */
#ifdef USE_CHARMPP
		static bool set_ccs( const ObjId& dest, const string& field, 
			         A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6){
                  unsigned int size;
                  SetGet6::Args wrapper(dest, field, arg1, arg2, arg3, arg4, arg5, arg6);
                  char *msg = CcsPackUnpack< SetGet6::Args >::pack(wrapper, size);
                  CcsSendBroadcastRequest(&SetGet::ccsServer_, SetGet6::setHandlerString(), size, msg);
                  delete[] msg;

                  bool ret;
                  while(CcsRecvResponse(&SetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
                  return ret;
                }

                static void set_handler(char *msg){
                  SetGet6< A1, A2, A3, A4, A5, A6 >::Args args;
                  CcsPackUnpack< SetGet6< A1, A2, A3, A4, A5, A6 >::Args >::unpack(msg, args);
                  CmiFree(msg);

                  bool ret = set(args.dest_, args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_, args.a6_);
                  CcsSendReply(sizeof(bool), &ret);
                }

#endif

		static bool set( const ObjId& dest, const string& field, 
			                  A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6 )
		{
			SetGet6< A1, A2, A3, A4, A5, A6 > sg( dest );
			FuncId fid;
			ObjId tgt( dest );
			if ( const OpFunc* func = sg.checkSet( field, tgt, fid ) ) {
				Conv< A1 > conv1( arg1 );
				Conv< A2 > conv2( arg2 );
				Conv< A3 > conv3( arg3 );
				Conv< A4 > conv4( arg4 );
				Conv< A5 > conv5( arg5 );
				Conv< A6 > conv6( arg6 );

				unsigned int totSize = 
					conv1.size() + conv2.size() + 
					conv3.size() + conv4.size() + conv5.size();
				double* temp = new double[ totSize ];
				double* ptr = temp;
				conv1.val2buf( ptr ); ptr += conv1.size();
				conv2.val2buf( ptr ); ptr += conv2.size();
				conv3.val2buf( ptr ); ptr += conv3.size();
				conv4.val2buf( ptr ); ptr += conv4.size();
				conv5.val2buf( ptr ); ptr += conv5.size();
				conv6.val2buf( ptr );
				func->op( tgt.eref(), &qi_, temp );

				delete[] temp;

				return 1;
			}
			return 0;
		}

		/**
		 * Blocking call using string conversion.
		 * As yet we don't have 2 arg conversion from a single string.
		 * So this is a dummy
		 */
		static bool innerStrSet( const ObjId& dest, const string& field, 
			const string& val )
		{
			A1 arg1;
			A2 arg2;
			A3 arg3;
			A4 arg4;
			A5 arg5;
			A6 arg6;
			string::size_type pos = val.find_first_of( "," );
			Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
			string temp = val.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A4 >::str2val( arg4, temp.substr( 0, pos ) );
			temp = temp.substr( pos + 1 );
			pos = temp.find_first_of( "," );
			Conv< A5 >::str2val( arg5, temp.substr( 0, pos ) );
			Conv< A6 >::str2val( arg6, temp.substr( pos + 1 ) );
			return set( dest, field, arg1, arg2, arg3, arg4, arg5, arg6 );
		}
	//////////////////////////////////////////////////////////////////
	//  The 'Get' calls for 2 args are currently undefined.
	//////////////////////////////////////////////////////////////////
	
		/**
		 * Terminating call using string conversion
		 */
		string harvestStrGet() const
		{ 
			return "";
		}
};

#endif // _SETGET_H
