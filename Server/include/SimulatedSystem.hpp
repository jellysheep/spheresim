/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SIMULATEDSYSTEM_HPP_
#define _SIMULATEDSYSTEM_HPP_

#include <Vector.hpp>
#include <Wall.hpp>

#include <QList>

class QDataStream;

namespace SphereSim
{
	/** \brief Storage for physical constants, wall properties and other parameters of the simulated system. */
	class SimulatedSystem
	{
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
		
		/** \brief 2D walls placed in the system. */
		QList<Wall2D> walls2D;
		
		/** \brief 3D walls placed in the system. */
		QList<Wall3D> walls3D;
		
		Scalar maximumTheta;
	};
}

#endif
