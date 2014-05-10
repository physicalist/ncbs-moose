///////////////////////////////////////////////////////////
//  PN2S_HelperFunctions.h
//  Implementation of the Helper functions
//  Created on:      26-Dec-2013 4:18:18 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_8104B80E_EF46_47df_2AB8_FC787EF2223C__INCLUDED_)
#define EA_8104B80E_EF46_47df_2AB8_FC787EF2223C__INCLUDED_

#include <math.h>
#include <algorithm>
#include <string>

#include <vector>
using namespace std;

template <typename T_ELEM>
void _printMatrix_Column(int m, int n, T_ELEM* A) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			cout << A[i + j *n ] << " ";
		}
		cout << endl;
	}
	cout << endl << flush;
}

template <typename T_ELEM>
void _printMatrix(int m, int n, T_ELEM* A) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			cout << A[i*n + j ] << " ";
		}
		cout << endl;
	}
	cout << endl << flush;
}

void inline _printMatrix(vector< vector< double > >& matrix)
{
	vector< double >::iterator icc;
	for ( vector< vector< double > >::iterator ic = matrix.begin(); ic != matrix.end(); ++ic ) {
		for ( icc = ic->begin(); icc != ic->end(); ++icc ) {
			cout << *icc << "\t";
		}
		cout << endl;
	}
	cout <<endl<<flush;
}
template <typename T_ELEM>
void _printVector(int n, T_ELEM* A) {
	for (int j = 0; j < n; ++j) {
		cout << A[j] << ", ";
	}
	cout << endl << flush;
}

void inline _printVector(vector< double >& vec)
{
	vector< double >::iterator icc;
	for ( icc = vec.begin(); icc != vec.end(); ++icc ) {
		cout << *icc << "\t";
	}
	cout << endl<<endl<<flush;
}
void inline _printVector(vector< uint >& vec)
{
	vector< uint >::iterator icc;
	for ( icc = vec.begin(); icc != vec.end(); ++icc ) {
		cout << *icc << "\t";
	}
	cout << endl<<endl<<flush;
}

#endif //! defined(EA_8104B80E_EF46_47df_2AB8_FC787EF2223C__INCLUDED_)
