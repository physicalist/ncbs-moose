#ifndef _GPU_INTERFACE_H
#define _GPU_INTERFACE_H

#include "../../basecode/header.h"			// For miscellaneous defns
#include "../HSolveStruct.h"				// For structure definitions
#include "../RateLookup.h"
#include "../HinesMatrix.h"					// For JunctionStruct
#include "../HSolvePassive.h"
#include "../HSolveActive.h"
#include "../HSolve.h"						// For HSolve


void rand_func()
{
	unsigned int ic = 0;
    vector< double >::iterator ihs = HS_.begin();
    vector< vdIterator >::iterator iop = operand_.begin();
    vector< JunctionStruct >::iterator junction;

    double pivot;
    double division;
    unsigned int index;
    unsigned int rank;
    for ( junction = junction_.begin();
            junction != junction_.end();
            junction++ )
    {
        index = junction->index;
        rank = junction->rank;

        while ( ic < index )
        {
            *( ihs + 4 ) -= *( ihs + 1 ) / *ihs **( ihs + 1 );
            *( ihs + 7 ) -= *( ihs + 1 ) / *ihs **( ihs + 3 );

            ++ic, ihs += 4;
        }

        pivot = *ihs;
        if ( rank == 1 )
        {
            vdIterator j = *iop;
            vdIterator s = *( iop + 1 );

            division    = *( j + 1 ) / pivot;
            *( s )     -= division **j;
            *( s + 3 ) -= division **( ihs + 3 );

            iop += 3;
        }
        else if ( rank == 2 )
        {
            vdIterator j = *iop;
            vdIterator s;

            s           = *( iop + 1 );
            division    = *( j + 1 ) / pivot;
            *( s )     -= division **j;
            *( j + 4 ) -= division **( j + 2 );
            *( s + 3 ) -= division **( ihs + 3 );

            s           = *( iop + 3 );
            division    = *( j + 3 ) / pivot;
            *( j + 5 ) -= division **j;
            *( s )     -= division **( j + 2 );
            *( s + 3 ) -= division **( ihs + 3 );

            iop += 5;
        }
        else
        {
            vector< vdIterator >::iterator
            end = iop + 3 * rank * ( rank + 1 );
            for ( ; iop < end; iop += 3 )
                **iop -= **( iop + 2 ) / pivot ***( iop + 1 );
        }

        ++ic, ihs += 4;
    }

    while ( ic < nCompt_ - 1 )
    {
        *( ihs + 4 ) -= *( ihs + 1 ) / *ihs **( ihs + 1 );
        *( ihs + 7 ) -= *( ihs + 1 ) / *ihs **( ihs + 3 );

        ++ic, ihs += 4;
    }
}
#endif