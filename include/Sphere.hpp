/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include <Vector.hpp>

class QDataStream;

namespace SphereSim
{
	/** \brief Physical data of a sphere. */
	class Sphere
	{
	public:
		/** \brief Sphere position (in metres). */
		Vector3 pos;
		/** \brief Sphere speed (in metres per second). */
		Vector3 speed;
		/** \brief Sphere acceleration (in metres per second squared). */
		Vector3 acc;
		/** \brief Sphere radius (in metres). */
		Scalar radius;
		/** \brief Sphere mass (in kilograms). */
		Scalar mass;
		
		/** \brief Initialize Sphere with position, speed, acceleration, radius and mass. */
		Sphere(Vector3 x, Vector3 v, Vector3 a, Scalar r, Scalar m)
		{
			pos = x;
			speed = v;
			acc = a;
			radius = r;
			mass = m;
		}
		
		/** \brief Initialize default Sphere. */
		Sphere():Sphere(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0),0,0)
		{
		}
	};
}

#endif
