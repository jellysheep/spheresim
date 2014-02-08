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
			/** \brief Heun-Euler method of order 2 and 1 using 2 force evaluations. */
			HeunEuler21,
			/** \brief Bogacki-Shampine method of order 3 and 2 using 4 force evaluations. */
			BogackiShampine32,
			/** \brief Runge-Kutta-Fehlberg method of order 5 and 4 using 6 force evaluations. */
			RungeKuttaFehlberg54,
			/** \brief Cash-Karp method of order 5 and 4 using 6 force evaluations. */
			CashKarp54,
			/** \brief Dormand-Prince method of order 5 and 4 using 7 force evaluations. */
			DormandPrince54
		};
	}
}

#endif
