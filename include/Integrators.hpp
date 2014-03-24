/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _INTEGRATORS_HPP_
#define _INTEGRATORS_HPP_

#include <QTextStream>

namespace SphereSim
{

	/** \brief Methods for numerically integrating differential equations. */
	namespace IntegratorMethods
	{
		/** \copydoc IntegratorMethods */
		enum Method
		{
			/** \brief Heun-Euler of order 2+1 using 2 evaluations. */
			HeunEuler21,
			/** \brief Bogacki-Shampine of order 3+2 using 4 evaluations. */
			BogackiShampine32,
			/** \brief Runge-Kutta-Fehlberg of order 5+4 using 6 evaluations. */
			RungeKuttaFehlberg54,
			/** \brief Cash-Karp of order 5+4 using 6 evaluations. */
			CashKarp54,
			/** \brief Dormand-Prince of order 5+4 using 7 evaluations. */
			DormandPrince54
		};
	}
}

#endif
