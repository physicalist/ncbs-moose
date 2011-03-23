// utility.h --- 
// 
// Filename: utility.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Wed Mar 23 10:25:58 2011 (+0530)
// Version: 
// Last-Updated: Wed Mar 23 10:32:34 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 13
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 
// 

// Code:

#ifndef _UTILITY_H

char shortType(string type);
char shortFinfo(string ftype);

void tokenize(
	const string& str,	
	const string& delimiters,
        vector< string >& tokens );
string trim(const std::string myString);

#endif // !_UTILITY_H


// 
// utility.h ends here
