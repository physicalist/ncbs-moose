/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _READ_KKIT_H
#define _READ_KKIT_H

class ReadKkit
{
	public: 
		enum ParseMode {
			DATA,
			INIT,
			COMMENT,
			LINE_CONTINUE
		};

		ReadKkit();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////
		double getMaxTime() const;
		double getPlotDt() const;
		double getDefaultVol() const;
		string getBasePath() const;
		unsigned int getVersion() const;

		//////////////////////////////////////////////////////////////////
		// Undump operations
		//////////////////////////////////////////////////////////////////
		
		void innerRead( ifstream& fin );
		ParseMode readInit( const string& line );
		Id read( const string& filename, const string& cellname, 
			Id parent, const string& solverClass = "Stoich" );
		void readData( const string& line );
		void undump( const vector< string >& args );

		/**
		 * This function sets up the kkit model for a run using the GSL,
		 * which means numerical integration using the GSL, all the plots
		 * specified by the kkit file, and the timestep for plots as 
		 * specified by the kkit file.
		 */
		// void setupGslRun();

		void run();
		void dumpPlots( const string& filename );

		//////////////////////////////////////////////////////////////////
		// Building up the model
		//////////////////////////////////////////////////////////////////
		Id buildCompartment( const vector< string >& args );
		Id buildPool( const vector< string >& args );
		Id buildReac( const vector< string >& args );
		Id buildEnz( const vector< string >& args );
		Id buildPlot( const vector< string >& args );
		Id buildTable( const vector< string >& args );
		unsigned int loadTab(  const vector< string >& args );
		Id buildGroup( const vector< string >& args );
		Id buildText( const vector< string >& args );
		Id buildGraph( const vector< string >& args );
		Id buildGeometry( const vector< string >& args );
		Id buildStim( const vector< string >& args );
		Id buildChan( const vector< string >& args );
		Id buildInfo( Id parent, map< string, int >& m, 
			const vector< string >& args );
		void buildSumTotal( const string& src, const string& dest );
		/**
 		* Finds the source pool for a SumTot. It also deals with cases where
 		* the source is an enz-substrate complex
 		*/
		Id findSumTotSrc( const string& src );

		//////////////////////////////////////////////////////////////////
		// Special ops in the model definition
		//////////////////////////////////////////////////////////////////
		void addmsg( const vector< string >& args );
		void setupSlaveMsg( const string& src, const string& dest );
		void innerAddMsg( 
			const string& src, const map< string, Id >& m1, 
				const string& srcMsg,
			const string& dest, const map< string, Id >& m2, 
				const string& destMsg,
			bool isBackward = 0 );
		void call( const vector< string >& args );
		void objdump( const vector< string >& args );
		void textload( const vector< string >& args );
		void separateVols( Id pool, double vol );
		void assignPoolCompartments();

		/**
		 * Goes through all Reacs and connects them up to each of the
		 * compartments in which one or more of their reactants resides.
		 * Thus, if any of these compartments changes volume, the Reac will
		 * be informed.
		 */     
		void assignReacCompartments();
		void assignEnzCompartments();
		void assignMMenzCompartments();

		/**
		 * We have a slight problem because MOOSE has a more precise value
		 * for NA than does kkit. Also, at the time the model is loaded,
		 * the volume relationships are unknown. So we need to fix up conc
		 * units of all pools reacs post-facto.
		 * Here we assume that the conc units from Kkit are
		 * meant to be OK, so they override the #/cell (lower case k) units.
		 * So we convert all the Kfs and Kbs in the entire system after
		 * the model has been created, once we know the order of each reac.
		 */
		void convertParametersToConcUnits();

		/// Convert pool amounts. Initially given in n, but scaling issue.
		void convertPoolAmountToConcUnits();

		/// Convert Reac rates, initially in n units.
		void convertReacRatesToConcUnits();

		/**
		 * Convert MMEnz rates. Km should be in conc units. Actually rates
		 * stored as k1, k2, k3 in num units, so this has to be computed
		 * and then converted.
		 */
		void convertMMenzRatesToConcUnits();

		/**
		 * Convert regular Enz rates. Binding step k1 has similar issues 
		 * as reac rates. k2 and k3 are both in units of 1/time, so OK.
		 */
		void convertEnzRatesToConcUnits();

		//////////////////////////////////////////////////////////////////
		// Utility functions
		//////////////////////////////////////////////////////////////////
		
		/**
		 * Splits up kkit path into head and tail portions, 
		 * tail is returned.
		 * Note that this prepends the basePath to the head.
		 */
		string pathTail( const string& path, string& head ) const;

		/**
		 * Utility function. Cleans up path strings. In most cases, it
		 * replaces things with underscores.
		 * Replaces square braces with underscores.
		 * Replaces '*' with 'p' 
		 *         as it is usually there to indicate phosphorylation
		 * Replaces '-' with underscore
		 * Replaces '@' with underscore
		 * Replaces ' ' with underscore
		 */
		string cleanPath( const string& path ) const;

		/**
		 * make kinetics and graphs elements.
		 */
		void makeStandardElements();

	private:
		string basePath_; /// Base path into which entire kkit model will go
		Id baseId_; /// Base Id onto which entire kkit model will go.

		double fastdt_; /// fast numerical timestep from kkit.
		double simdt_;	/// regular numerical timestep from kkit.
		double controldt_;	/// Timestep for updating control graphics
		double plotdt_;		/// Timestep for updating plots
		double maxtime_;	/// Simulation run time.
		double transientTime_;	/// Time to run model at fastdt
		bool useVariableDt_;	/// Use both fast and sim dts.
		double defaultVol_;		/// Default volume for new compartments.
		unsigned int version_;	/// KKit version.
		unsigned int initdumpVersion_;	/// Initdump too has a version.

		unsigned int numCompartments_;
		unsigned int numPools_;
		unsigned int numReacs_;
		unsigned int numEnz_;
		unsigned int numMMenz_;
		unsigned int numPlot_;
		unsigned int numStim_;
		unsigned int numOthers_;

		unsigned int lineNum_;

		map< string, int > poolMap_;
		map< string, int > reacMap_;
		map< string, int > enzMap_;
		map< string, int > groupMap_;
		map< string, int > tableMap_;
		map< string, int > stimMap_;
		map< string, Id > poolIds_;
		map< string, Id > reacIds_;
		map< string, Id > enzIds_;
		map< string, Id > mmEnzIds_;
		map< string, Id > plotIds_;
		map< string, Id > tabIds_;
		map< string, Id > stimIds_;
		map< string, Id > chanIds_;

		/*
		vector< Id > pools_;
		/// This keeps track of all vols, since the pools no longer do.
		vector< double > poolVols_;
		*/

		/// This keeps track of unique volumes
		vector< double > vols_;

		/// This keeps track of the last Table used in loadtab
		Id lastTab_;
		/// This holds the vector of array entries for loadtab
		vector< double > tabEntries_;

		/// List of Ids in each unique volume.
		vector< vector< Id > > volCategories_;
		vector< Id > compartments_;
		vector< pair< Id, Id > > enzCplxMols_;

		map< Id, int > poolFlags_;

		map< Id, double > poolVols_; // Need for enz complexes.

		Shell* shell_;

		static const double EPSILON;
};

#endif // READ_KKIT_H
