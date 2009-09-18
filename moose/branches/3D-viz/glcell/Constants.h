/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

const double SIZE_EPSILON = 1e-8; // floating-point (FP) epsilon for 
                                  // ... minimum compartment size
const double FP_EPSILON = 1e-8;   // FP epsilon for comparison

const int WINDOW_OFFSET_X = 50;
const int WINDOW_OFFSET_Y = 50;
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

const char SYNCMODE_ACKCHAR = '*';

#endif // CONSTANTS_H
