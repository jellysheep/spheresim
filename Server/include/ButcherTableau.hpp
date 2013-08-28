/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _BUTCHERTABLEAU_HPP_
#define _BUTCHERTABLEAU_HPP_

#include <Vector.hpp>
#include <Console.hpp>

#include <QtGlobal>

class QTcpServer;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Defines the Butcher tableau values for a Runge-Kutta method.
	 */
	class ButcherTableau{
	public:
		/** \brief Holds the Runge-Kutta method order and defines the array sizes. */
		quint8 order;
		/** \brief Holds the central matrix. */
		Scalar a[10][10];
		/** \brief Holds the first bottom vector. */
		Scalar b[10];
		/** \brief Holds the second bottom vector. */
		Scalar b_[10];
		/** \brief Holds the left vector. */
		Scalar c[10];
		
		ButcherTableau();
		/**
		 * \brief Initializes the Butcher tableau.
		 * \param _order The Runge-Kutta method order and array size.
		 * \param _a The central matrix.
		 * \param _b The first bottom vector.
		 * \param _b_ The second bottom vector.
		 * \param _c The left vector.
		 */
		ButcherTableau(quint8 _order, const Scalar* _a, const Scalar* _b, const Scalar* _b_, const Scalar* _c);
	};
	
}

#endif
