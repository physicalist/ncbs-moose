/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CHEM_COMPT_H
#define _CHEM_COMPT_H

class ChemCompt: public Data
{
	public: 
		ChemCompt();
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setSize( double v );
		double getSize() const;

		void setDimensions( unsigned int v );
		unsigned int getDimensions() const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void process( const ProcInfo* p, const Eref& e );

		void extent( DataId di, double volume, double area, double perimeter );
		//////////////////////////////////////////////////////////////////
		// Lookup funcs for Boundary
		//////////////////////////////////////////////////////////////////
		Boundary* lookupBoundary( unsigned int index );
		void setNumBoundary( unsigned int num );
		unsigned int getNumBoundary( ) const;

		// bool isInside( double x, double y, double z );

		static const Cinfo* initCinfo();
	private:
		double size_;
		unsigned int dimensions_;

		/**
		 * The Boundaries are Element Fields. They appear as distinct
		 * Elements, though they are controlled by the ChemCompt.
		 * These are the interfaces between compartments, or just
		 * the boundaries of the current one. Each Boundary can be
		 * diffusive, reflective, or an interface where molecules in
		 * different compartments can talk to each other.
		 * All boundaries have a message to a Geometry. The Geometries
		 * may be shared, which is why the boundary isn't a Geometry itself.
		 * If it is an interface (diffusive or other) then the boundary 
		 * also contains a message to the adjacent compartment.
		 */
		vector< Boundary > boundaries_;
};

#endif	// _CHEM_COMPT_H
