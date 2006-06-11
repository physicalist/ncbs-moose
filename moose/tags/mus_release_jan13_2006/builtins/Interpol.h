#ifndef _Interpol_h
#define _Interpol_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
class Interpol
{
	friend class InterpolWrapper;
	public:
		Interpol()
		{
			xmin_ = 0.0;
			xmax_ = 1.0;
			mode_ = 0;
			table_.resize( 2, 0.0 );
			table_[0] = 1;
			table_[1] = 3.1415926535;
		}
		double localGetXmin() const {
			return xmin_;
		}
		double localGetXmax()const {
			return xmax_;
		}
		int localGetXdivs() const {
			return table_.size() - 1;
		}
		double interpolateWithoutCheck( double x ) const;
		double indexWithoutCheck( double x ) const {
			return table_[ static_cast< int >( (x - xmin_) * invDx_) ];
		}
		double doLookup( double x ) const;
		bool operator==( const Interpol& other ) const;
		bool operator<( const Interpol& other ) const;
		void localSetXmin( double value );
		void localSetXmax( double value );
		void localSetXdivs( int value );
		// Later do interpolation etc to preseve contents.
		void localSetDx( double value );
		double localGetDx() const;

	private:
		double xmin_;
		double xmax_;
		int mode_;
		vector < double > table_;
		double invDx_;
		static const double EPSILON;
		static const int MAX_DIVS;
};
#endif // _Interpol_h
