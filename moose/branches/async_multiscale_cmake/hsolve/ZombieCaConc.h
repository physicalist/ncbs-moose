/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_CACONC_H
#define _ZOMBIE_CACONC_H
#include "header.h"
#include "ElementValueFinfo.h"
#include "../biophysics/CaConc.h"
#include "HinesMatrix.h"
#include "HSolveStruct.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"

class DataHandler;

/**
 * Zombie object that lets HSolve do its calculations, while letting the user
 * interact with this object as if it were the original object.
 */
class ZombieCaConc
{
public:
    ZombieCaConc()
        :
        hsolve_( NULL ),
        tau_( 0.0 ),
        B_( 0.0 ),
        thickness_( 0.0 )
    {
        ;
    }

    ///////////////////////////////////////////////////////////////
    // Message handling functions
    ///////////////////////////////////////////////////////////////
    void reinit( const Eref&, ProcPtr info );
    void process( const Eref&, ProcPtr info );

    void current( double I );
    void currentFraction( double I, double fraction );
    void increase( double I );
    void decrease( double I );
    ///////////////////////////////////////////////////////////////
    // Field handling functions
    ///////////////////////////////////////////////////////////////
    void setCa( const Eref& e,   double val );
    double getCa( const Eref& e ) const;
    void setCaBasal( const Eref& e , double val );
    double getCaBasal( const Eref& e ) const;
    void setTau( const Eref& e , double val );
    double getTau( const Eref& e ) const;
    void setB( const Eref& e , double val );
    double getB( const Eref& e ) const;
    void setCeiling( const Eref& e , double val );
    double getCeiling( const Eref& e ) const;
    void setFloor( const Eref& e , double val );
    double getFloor( const Eref& e ) const;

    // Locally stored fields.
    void setThickness( double val );
    double getThickness() const;

    static const Cinfo* initCinfo();

    //////////////////////////////////////////////////////////////////
    // utility funcs
    //////////////////////////////////////////////////////////////////
    static void zombify( Element* solver, Element* orig );
    static void unzombify( Element* zombie );

    /*
     * This Finfo is used to send out CaConc to channels.
     * The original CaConc sends this itself, whereas the HSolve
     * sends on behalf of the Zombie.
     */
    static SrcFinfo1< double >* concOut();
private:
    HSolve* hsolve_;

    double tau_;
    double B_;
    double thickness_;

    void copyFields( CaConc* c );
};


#endif // _ZOMBIE_CACONC_H
