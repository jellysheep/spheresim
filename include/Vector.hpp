
#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

//#define USE_EIGEN_LIBRARY 1
//#define USE_DOUBLE 1

#if USE_DOUBLE
	#define FLOATING_TYPE double
#else
	#define FLOATING_TYPE float
#endif
typedef FLOATING_TYPE Scalar;

#if USE_EIGEN_LIBRARY
	//#define NDEBUG
	#include <Eigen/Dense>
	typedef Eigen::Matrix<FLOATING_TYPE, 1, 1> Vector1;
	typedef Eigen::Matrix<FLOATING_TYPE, 2, 1> Vector2;
	typedef Eigen::Matrix<FLOATING_TYPE, 3, 1> Vector3;
	typedef Eigen::Matrix<FLOATING_TYPE, 4, 1> Vector4;
#else
	#include <VectorLib.hpp>
	typedef LibVector<FLOATING_TYPE, 1> Vector1;
	typedef LibVector<FLOATING_TYPE, 2> Vector2;
	typedef LibVector<FLOATING_TYPE, 3> Vector3;
	typedef LibVector<FLOATING_TYPE, 4> Vector4;
#endif

#endif
