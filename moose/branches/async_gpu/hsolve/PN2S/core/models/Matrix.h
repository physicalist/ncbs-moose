///////////////////////////////////////////////////////////
//  Matrix.h
//
//  Created on:      26-Dec-2013 4:20:54 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A905F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_)
#define A905F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_

#include <vector>

// workaround issue between gcc >= 4.6 and cuda 6.0
#if (defined __GNUC__) && (__GNUC__>4 || __GNUC_MINOR__>=6)
  #undef _GLIBCXX_ATOMIC_BUILTINS
  #undef _GLIBCXX_USE_INT128
#endif

#include <Eigen/Core>

namespace pn2s
{
namespace models
{

typedef Eigen::MatrixXd Matrix;

//template <typename T>
//class Matrix : public Ei
//{
//	int _n;
//	int _m;
//public:
//	std::vector< std::vector<T> > _data;
//	unsigned int gid;
//
//	Matrix();
//	Matrix(int n, int m);
//	virtual ~Matrix();
//
////	Matrix( const Matrix<T>& other );
//	Matrix<T>& operator=(Matrix<T> rhs);
//
//	std::vector<T> operator [](int i) const {return _data[i];}
//	std::vector<T> & operator [](int i) {return _data[i];}
//};

}
}
#endif // !defined(A905F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_)
