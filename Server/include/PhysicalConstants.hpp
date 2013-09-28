/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _PHYSICALCONSTANTS_HPP_
#define _PHYSICALCONSTANTS_HPP_

#include <Vector.hpp>

class QDataStream;

namespace SphereSim{
	/**
	 * \brief Holds physical constants.
	 */
	class PhysicalConstants{
	public:
		/** \brief E modulus of the spheres. */
		Scalar E_sphere;
		/** \brief Poisson number of the spheres. */
		Scalar poisson_sphere;
		
		/** \brief E modulus of the walls. */
		Scalar E_wall;
		/** \brief Poisson number of the walls. */
		Scalar poisson_wall;
		
		/** \brief E modulus (E*) of a sphere-wall collision. */
		Scalar E_sphere_wall;
		/** \brief E modulus (E*) of a sphere-sphere collision. */
		Scalar E_sphere_sphere;
		
		/** \brief Earth gravity. */
		Vector3 earthGravity;
	};
}

#endif
