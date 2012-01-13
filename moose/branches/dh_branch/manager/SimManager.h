/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class SimManager
{
	public:
		enum TreeType { CHEM_ONLY, KKIT, CHEM_SPACE, CHEM_SPACE_MULTISOLVER, SIGNEUR };

		SimManager();
		~SimManager();

		// Field Definitions
		void setAutoPlot( bool v );
		bool getAutoPlot() const;

		void setSyncTime( double v );
		double getSyncTime() const;

		/// Destination function
		void build( const Eref& e, const Qinfo* q, string method );
		void makeStandardElements( const Eref& e, const Qinfo*q, string meshClass );

		// Utility functions
		TreeType findTreeType( const Eref& e );
		void buildFromBareKineticTree( const string& method );
		void buildFromKkitTree( const string& method );

		static const Cinfo* initCinfo();
	private:
		/// syncTime is the interval between synchronizing various solvers.
		double syncTime_;

		/** 
		 * When autoPlot is true, the SimManager builds plots automatically
		 * for fields that it thinks are of interest
		 */
		bool autoPlot_;

		Id baseId_;

		double plotdt_;
};
