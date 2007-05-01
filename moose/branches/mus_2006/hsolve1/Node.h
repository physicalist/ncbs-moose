/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _Node_h
#define _Node_h

template < class T >
struct Node {
	T parent;
	T self;
	vector< T > child;
	unsigned long state;
	unsigned long label;
};

#endif
