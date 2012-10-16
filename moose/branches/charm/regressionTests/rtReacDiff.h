#ifndef MOOSE_RT_REAC_DIFF_H
#define MOOSE_RT_REAC_DIFF_H

#ifdef USE_CHARMPP
class ElementContainer;
#endif

class TestReacDiff {
#ifdef USE_CHARMPP
  ElementContainer *container_;
#endif

  public:

#ifndef USE_CHARMPP
  TestReacDiff() {}
#else
  TestReacDiff(ElementContainer *container) : 
    container_(container)
  {}
#endif

    /**
     * This regression test takes a simple model and replicates it over a
     * 2x2x2 cube mesh. Tests the initial creation and replication of model,
     * and that the duplicates all run properly.
     * Also tests ReadCspace.
     */
    private:
    void rtReplicateModels();
    double checkDiff( const vector< double >& conc, double D, double t, double dx);
    void testDiff1D();
    double checkNdimDiff( const vector< double >& conc, double D, double t, 
        double dx, double n, unsigned int cubeSide );
    void testDiffNd( unsigned int n );
    void testReacDiffNd( unsigned int n );
 
  public:
    void rtReacDiff();
};

#endif // MOOSE_RT_REAC_DIFF_H
