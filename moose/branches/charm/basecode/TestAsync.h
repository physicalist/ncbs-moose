#ifndef MOOSE_TEST_ASYNC_H
#define MOOSE_TEST_ASYNC_H

#include "header.h"
#include <stdio.h>
#include <iomanip>
#include "../shell/Neutral.h"
#include "../builtins/Arith.h"
#include "../builtins/Mdouble.h"
#include "Dinfo.h"
#include <queue>
#include "../biophysics/Synapse.h"
#include "../biophysics/SynBase.h"
#include "../biophysics/IntFire.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "SingleMsg.h"
#include "OneToOneMsg.h"
#include "../randnum/randnum.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"

#include "../shell/Shell.h"

class TestId {
  public :
    void setId( Id id ) {
      id_ = id;
    }
    Id getId() const {
      return id_;
    }

 // Here we test setRepeat using an Id field. This test is added
  // because of a memory leak problem that cropped up much later.
    static const Cinfo* initCinfo() {
      static ValueFinfo< TestId, Id > id(
          "id",
          "test",
          &TestId::setId,
          &TestId::getId
          );
      static Finfo* testIdFinfos[] = {&id};
      static Cinfo testIdCinfo(
          "TestIdRepeatAssignment",
          Neutral::initCinfo(),
          testIdFinfos,
          sizeof( testIdFinfos )/ sizeof( Finfo* ),
          new Dinfo< TestId >()
          );
      return &testIdCinfo;
    }


  private :
    Id id_ ;
};

/**
 * This sets up a reciprocal shared Msg in which the incoming value gets
 * appended onto the corresponding value of the target. Also, as soon
 * as any of the s1 or s2 are received, the target sends out an s0 call.
 * All this is tallied for validating the unit test.
 */

class Test
{
  public:
  static SrcFinfo0 s0;

  public:
    Test()
      : numAcks_( 0 )
    {;}

    void process( const Eref& e, ProcPtr p )
    {;}

    void handleS0() {
      numAcks_++;
    }

    void handleS1( const Eref& e, const Qinfo* q, string s ) {
      s_ = s + s_;
      ThreadId threadNum = 0;
      s0.send( e, threadNum );
    }

    void handleS2( const Eref& e, const Qinfo* q, int i1, int i2 ) {
      i1_ += 10 * i1;
      i2_ += 10 * i2;
      ThreadId threadNum = 0;
      s0.send( e, threadNum );
    }

    static Finfo* sharedVec[ 6 ];

    static const Cinfo* initCinfo()
    {
      static SharedFinfo shared( "shared", "",
          sharedVec, sizeof( sharedVec ) / sizeof( const Finfo * ) );
      static Finfo * testFinfos[] = {
        &shared,
      };

      static Cinfo testCinfo(
          "Test",
          0,
          testFinfos,
          sizeof( testFinfos ) / sizeof( Finfo* ),
          new Dinfo< Test >()
          );

      return &testCinfo;
    }

    string s_;
    int i1_;
    int i2_;
    int numAcks_;
};


  void testAsync( );
  void showFields();
  void testPrepackedBuffer();
  /**
   * This used to use the 'name' field of Neutral as a test variable.
   * Now no longer as useful, since the field is replaced with the 'name'
   * of the parent Element. Instead use Arith.output.
   */
  void insertIntoQ( );
  void testSendMsg();
  // This used to use parent/child msg, but that has other implications
  // as it causes deletion of elements.
  void testCreateMsg();
  /**
   * This tests the low-level Set functions, that do not involve off-node
   * messaging.
   */
  void testInnerSet();
  void testInnerGet(); // Uses low-level ops to do a 'get'.
  void testSetGet();
  void testStrSet();
  void testGet();
  void testStrGet();

  void testSetGetDouble();
  void testSetGetSynapse();
  void testSetGetVec();
  void test2ArgSetVec();
  void testFastGet();
  void testSetRepeat();
  void testSendSpike();
  void printSparseMatrix( const SparseMatrix< unsigned int >& m);
  void testSparseMatrix();
  void testSparseMatrix2();
  /** 
   * Need to recast this for the intact version of sparse matrix 
   * balancing.
   */
  void testSparseMatrixBalance();
  void printGrid( Element* e, const string& field, double min, double max );

  void testSparseMsg();
  void testUpValue();


  void testSharedMsg();
  void testConvVector();
  void testConvVectorOfVectors();
  void testMsgField();
  void testSetGetExtField();
  void testLookupSetGet();
  void testIsA();
  double* dptr( const DataHandler* dh, unsigned int i );
  void testDataCopyZero();
  void testDataCopyOne();

  void testDataCopyTwo();
  void testDataCopyAny();
  /*
     class testForeach: public Arith
     {
     public:
     testForeach() {
     vec_.resize( 0 );
     }

     pushBack( int i ) {
     vec_.push_back( i );
     }

     const vector< int >& vec() const {
     return vec_;
     }
     private:
     vector< int > vec_;
     }
   */

  class testForeach: public OpFuncDummy
  {
    public:
      testForeach( vector< double >* vec )
        : vec_( vec )
      {;}
      void op( const Eref& e, const Qinfo* q, const double* buf ) const
      {
        vec_->push_back( *buf );
      }

      const vector< double >& vec() const {
        return *vec_;
      }
    private:
      vector< double >* vec_;
  };

  void testOneDimHandler();
  void testFieldDataHandler();
  /**
   * Tests that a single copy of the FieldElement is made with all the
   * internal fields also copied over. This function examines the data
   * part of the copy.
   */
  void testCopyFieldElementData();
  void testFinfoFields();
  void testCinfoFields();
  void testCinfoElements();
  void testMsgSrcDestFields();


#endif // MOOSE_TEST_ASYNC_H
