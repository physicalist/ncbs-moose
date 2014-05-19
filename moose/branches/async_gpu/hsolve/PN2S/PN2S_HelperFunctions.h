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

struct Error_PN2S
{
    enum Type
    {
    	NO_ERROR = 0,
		CuBLASError = 1,
		CUDA_Error = 2
    };
    Type t_;
    string msg_;

    Error_PN2S() : t_(NO_ERROR), msg_("") {}
    Error_PN2S(Type t) : t_(t), msg_("") {}
    Error_PN2S(Type t, string msg) : t_(t), msg_(msg) {}
    operator Type () const {return t_;}
    string ErrorMsg() const {return msg_;}
private:
   //prevent automatic conversion for any other built-in types such as bool, int, etc
   template<typename T>
    operator T () const;
};

#define PN2S_SAFE_CALL(call)                                          \
do {                                                                  \
	Error_PN2S err = call;                                           \
    if (Error_PN2S::NO_ERROR != err) {                                         \
        fprintf (stderr, "PN2S error in file '%s' in line %i : %s.\n",\
                 __FILE__, __LINE__, err.ErrorMsg() );       \
        exit(EXIT_FAILURE);                                           \
    }                                                                 \
} while (0)

#define PN2S_CALL(call)                                          \
do {                                                                  \
	Error_PN2S err = call;                                           \
    if (Error_PN2S::NO_ERROR != err) {                                         \
        cerr << "PN2S error in file "<<__FILE__<<" in line " << __LINE__ << ": " << err.ErrorMsg();       \
        return (err);                                           \
    }                                                                 \
} while (0)


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
