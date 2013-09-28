/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <QtGlobal>

#if USE_DOUBLE
	#define FLOATING_TYPE qreal
#else
	#define FLOATING_TYPE float
#endif
typedef FLOATING_TYPE Scalar;

#if USE_EIGEN_LIBRARY
	//#define NDEBUG
	#include <Eigen/Dense>
	typedef Eigen::Matrix<Scalar, 1, 1> Vector1;
	typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
	typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
	typedef Eigen::Matrix<Scalar, 4, 1> Vector4;
#else
	#include <VectorLib.hpp>
	typedef LibVector<Scalar, 1> Vector1;
	typedef LibVector<Scalar, 2> Vector2;
	typedef LibVector<Scalar, 3> Vector3;
	typedef LibVector<Scalar, 4> Vector4;
#endif

#endif
