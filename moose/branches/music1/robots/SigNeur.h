/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SIGNEUR_H
#define _SIGNEUR_H


/**
 * enum of the different compartment types, worked out by analyzing the
 * cell model. Each of these will get assigned a distinct signaling model
 * and possibly different numberical method.
 */
enum CompartmentCategory { SOMA, DEND, SPINE, SPINE_NECK, EMPTY };

/**
 * Utility function for getting dimensions of electrical compt when it
 * is divided up into numSeg.
 */
void getSigComptSize( const Eref& compt, unsigned int numSeg,
	double& volume, double& xByL );

/**
 * Data structure that organizes the composite model.
 * Assumes arrays for each of the signaling models.
 */
class TreeNode {
	public:
		TreeNode( Id c, unsigned int p, CompartmentCategory cc )
			:
				compt( c ),
				parent( p ),
				sigStart( 0 ),
				sigEnd( 0 ),
				category( cc )
		{;}

		Id compt; /// Id of electrical compartment on this node.

		/// Parent compartment treeNode index. Empty for soma.
		unsigned int parent; 

		Id sigModel; /// Parent of array of sigModels.
		unsigned int sigStart; /// Indices of sigModel entries.
		unsigned int sigEnd; /// Indices of sigModel entries.

		/// type of compartment and hence sigModel to use.
		CompartmentCategory category; 
};

/**
 * SigNeur: A multiscale model builder, aka a robot.
 * This potentially interfaces 4 models: An electrical cell model, and
 * chemical models for spines, dendrites and soma. The latter may be
 * 3-D stochastic, ODE, and genetic respectively, but for now the
 * class only handles ODE versions.
 *
 * Constructs a copy of the cell model on itself. The cell has its own
 * cell manager. Then there is an array of dends, of spines and 
 * soma models. Each of these incorporates diffusion, which may be
 * between categories. The whole mess sits on a kinetic manager as it
 * will start out by solving it using rk5. A 300-compartment model will
 * have ~50,000 molecular species. This is about 10 times the biggest I
 * have tried so far.
 * There is also an array of adaptors, one per cell model compt.
 * In due course need to put a distinct kind of solver for the spines,
 * either 3D monte carlo or Gillespie. Fortunately these will be separated
 * and hence can run on distinct processors.
 */
class SigNeur
{
	public:
		SigNeur();
		
		///////////////////////////////////////////////////
		// Field assignment functions
		///////////////////////////////////////////////////
		
		// Object selection
		static void setCellProto( const Conn* c, Id value );
		static Id getCellProto( Eref e );
		static void setSpineProto( const Conn* c, Id value );
		static Id getSpineProto( Eref e );
		static void setDendProto( const Conn* c, Id value );
		static Id getDendProto( Eref e );
		static void setSomaProto( const Conn* c, Id value );
		static Id getSomaProto( Eref e );

		// These are readonly.
		static Id getCell( Eref e );
		static Id getSpine( Eref e );
		static Id getDend( Eref e );
		static Id getSoma( Eref e );
		
		// Numerical Method selection for electrical cell model.
		static void setCellMethod( const Conn* c, string value );
		static string getCellMethod( Eref e );
		static void setSpineMethod( const Conn* c, string value );
		static string getSpineMethod( Eref e );
		static void setDendMethod( const Conn* c, string value );
		static string getDendMethod( Eref e );
		static void setSomaMethod( const Conn* c, string value );
		static string getSomaMethod( Eref e );

		/// Timestep for signaling model
		static void setSigDt( const Conn* c, double value );
		static double getSigDt( Eref e );

		/// Timestep for Electrical model
		static void setCellDt( const Conn* c, double value );
		static double getCellDt( Eref e );

		/// Diffusion constant scaling factor, to globally modify diffusion
		static void setDscale( const Conn* c, double value );
		static double getDscale( Eref e );

		/// Characteristic length for linear diffusion.
		static void setLambda( const Conn* c, double value );
		static double getLambda( Eref e );

		/// Options for parallel configuration. 
		static void setParallelMode( const Conn* c, int value );
		static int getParallelMode( Eref e );

		/// Timestep at which to exchange data between elec and sig models.
		static void setUpdateStep( const Conn* c, double value );
		static double getUpdateStep( Eref e );

		/// Map between electrical and signaling channel representations
		static void setChannelMap( const Conn* c, 
			string val, const string& i );
		static string getChannelMap( Eref e, const string& i );

		/// Map between electrical and signaling Calcium representations
		static void setCalciumMap( const Conn* c, 
			string val, const string& i );
		static string getCalciumMap( Eref e, const string& i );

		/// Scale factors for calcium conversion between models.
		static void setCalciumScale( const Conn* c, double value );
		static double getCalciumScale( Eref e );

		/// Compartment names to include for dendrite signaling
		static void setDendInclude( const Conn* c, string value );
		static string getDendInclude( Eref e );

		/// Compartment names to exclude for dendrite signaling
		static void setDendExclude( const Conn* c, string value );
		static string getDendExclude( Eref e );
		
		///////////////////////////////////////////////////
		// Dest function definitions
		///////////////////////////////////////////////////
		
		static void build( const Conn* c );
		void innerBuild( const Conn* c );

		///////////////////////////////////////////////////
		// Setup function definitions
		///////////////////////////////////////////////////
		bool traverseCell( Eref me );
		void schedule( Eref me );
		Id findSoma( const vector< Id >& compts );
		void buildTree( Id soma, const vector< Id >& compts );
		// void innerBuildTree( unsigned int parent, Eref e, int msg );
		void innerBuildTree( unsigned int parent, Eref paE, Eref e, 
			int msg1, int msg2 );
		void assignSignalingCompts();
		void makeSignalingModel( Eref e );
		void insertDiffusion( Element* base );

		/** 
		 * This function uses naming heuristics to decide which signaling 
		 * model belongs in which compartment. By default, it puts 
		 * spine signaling in all compartments which have 'spine' in 
		 * the name, except for those which have 'neck' or 'shaft as well.
		 * It puts soma signaling in compartments with soma in the name,
		 * and dend signaling everywhere else. In addition, it has two
		 * optional fields to use: dendInclude and dendExclude. If
		 * dendInclude is set, then it only puts dends in the specified
		 * compartments. Whether or not dendInclude is set, dendExclude
		 * eliminates dends from the specified compartments.
 		 */
		CompartmentCategory guessCompartmentCategory( Eref e );

		Element* copySig( Id kinId, Id proto, 
			const string& name, unsigned int num );
		/**
 		 * Traverses the signaling tree to build a map of molecule Elements 
 		 * looked up by name.
 		 */
		void buildMoleculeNameMap( Element* e, 
			map< string, Element* >& molMap );


		/**
		 * Connect up appropriate compartments in the soma. Note that
		 * soma never needs to connect to any other compartment type.
		 */
		void completeSomaDiffusion( vector< unsigned int >& junctions );

		/**
		 * Connect up diffusion for dendritic compartments. Most will
		 * connect to adjacent dend compartments, but some will
		 * connect to soma. So we need to check the somaMap for molecules
		 * that diffuse.
		 */
		void completeDendDiffusion( vector< unsigned int >& junctions );

		/**
		 * Connect up diffusion to and from spines. All spines connect
		 * to dend compartments.
		 */
		void completeSpineDiffusion( vector< unsigned int >& junctions );

		/**
		 * Traverses the cell tree to work out where the diffusion reactions
		 * must connect to each other. junction[i] is the index of the 
		 * compartment connected to compartment[i]. The indexing of 
		 * compartments themselves is first the soma block, then the
		 * dend block, then the spine block.
		 */
		void buildDiffusionJunctions( vector< unsigned int >& junctions );

		/**
		 * Sets up rates for diffusion between m0 and m1 via diff.
		 * diff is a child of m0.
		 */
		void diffCalc( Eref m0, Eref m1, Eref diff );

		/**
 		* This figures out dendritic segment dimensions. It assigns the 
 		* volumeScale for each signaling compt, and puts Xarea / len into
 		* each diffusion element for future use in setting up diffusion
		* rates.
 		*/
		void setComptVols( Eref compt, 
			map< string, Element* >& molMap,
			unsigned int index, unsigned int numSeg );

		/**
 		* setAllVols traverses all signaling compartments  in the model and
 		* assigns volumes.
 		* This must be called before completeDiffusion because the vols
 		* computed here are needed to compute diffusion rates.
		* It marches through the molecule name->ElementPointer maps to
		* do this.
 		*/
		void setAllVols();

		/**
		 * This traverses the map of cell to signalling info flow,
		 * that is, calcium influx, to set up adaptors.
		 */

		void makeCell2SigAdaptors();

		/**
		 * This traverses the map of signalling to cell info flow to
		 * set up adaptors. This would be for modulating channel
		 * conductances based on molecular events.
		 */
		void makeSig2CellAdaptors();

	private:
		Id cellProto_; /// Prototype cell electrical model
		Id spineProto_; /// Prototype spine signaling model
		Id dendProto_; /// prototype dendrite signaling model
		Id somaProto_; /// prototype soma signaling model
		Id cell_; /// cell electrical model base.
		Id spine_; /// spine signaling model array base
		Id dend_; /// dendrite signaling model array base
		Id soma_; /// soma signaling model array base

		unsigned int numSpine_; /// # of spine signaling compartments
		unsigned int numNeck_; /// # of neck compartments. Not for signaling
		unsigned int numDend_; /// # of dend signaling compartments
		unsigned int numSoma_; /// # of soma signaling compartments

		string cellMethod_;
		string spineMethod_;
		string dendMethod_;
		string somaMethod_;

		/**
		 * sigDt_ is the timestep for signaling. This should be as long
		 * as possible, because most signaling solvers use adaptive 
		 * timesteps. The length of sigDt_ should be limited only by
		 * how often you want to read out, and how often you want external
		 * input to affect it.
		 */
		double sigDt_;	
					
		/**
		 * cellDt_ is the timestep for the compartmental biophysics solver.
		 * This is typically 50 to 100 usec. We currently do not have
		 * variable dt solvers, but that may change. In that case we
		 * may be able to stretch this time too.
		 */
		double cellDt_;

		double Dscale_;	/// Diffusion scale factor.
		double lambda_; /// Length constant for diffusion.
		int parallelMode_; /// Later redo to something more friendly
		double updateStep_;
		map< string, string > channelMap_; /// Channel name mapping
			// Scale factors are worked out from gmax and CoInit.
			
		map< string, string > calciumMap_; /// Calcium pool name mapping

		/**
		 * Scaling factor between elec and sig. Note that for both the 
		 * channel and calcium maps, we do the actual conversion through
		 * an array of interface objects which handle scaling and offsets.
		 * The scaling for the channelMap is from gmax and CoInit, and 
		 * the baseline for calciumMap is CaBasal and CoInit, and it 
		 * uses calciumScale for scaling.
		 */
		double calciumScale_;	

		/** 
		 * If dendInclude is set, then it only puts dends in the specified
		 * compartments. Whether or not dendInclude is set, dendExclude
		 * eliminates dends from the specified compartments.
 		 */
		string dendInclude_;
		string dendExclude_;

		vector< TreeNode > tree_;

		/// Name to Molecule map for soma compartment signaling models.
		map< string, Element* > somaMap_;
		/// Name to Molecule map for dend compartment signaling models.
		map< string, Element* > dendMap_;
		/// Name to Molecule map for spine compartment signaling models.
		map< string, Element* > spineMap_;

		// Vector of volumes of all segments
		vector< double > volume_;
		// vector of cross section area divided by length of segment.
		vector< double > xByL_;
};

#endif // _KINETIC_MANAGER_H
