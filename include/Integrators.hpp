/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _INTEGRATORS_HPP_
#define _INTEGRATORS_HPP_

#include <QTextStream>

namespace SphereSim{
	
	/** \brief Defines methods for numerically solving differential equations. */
	namespace IntegratorMethods{
		enum Method{
			EulerCauchy,
			Midpoint,
			RungeKutta4,
			Leapfrog,
			SemiImplicitEuler
		};
	}
}

#endif
