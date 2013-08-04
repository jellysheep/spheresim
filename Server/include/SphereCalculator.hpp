/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SPHERECALCULATOR_HPP_
#define _SPHERECALCULATOR_HPP_

#include <Sphere.hpp>

#include <QVector>

namespace SphereSim{
	
	class SphereManager;
	
	/**
	 * \brief Calculates sphere physics.
	 */
	class SphereCalculator{
	private:
		/** \brief Holds the spheres managed by the server. */
		QVector<Sphere> spheres;
		/** \brief Holds the spheres in an array. */
		Sphere* sphArr;
		/** \brief Holds the current spheres count. */
		int sphCount;
		/** \brief Holds the current box size. */
		Vector3 boxSize;
		/** \brief Holds the current step length (time in s). */
		Scalar timeStep;
		
		/**
		 * \brief Gets the spheres managed by the server.
		 * \return Spheres managed by the server.
		 */
		QVector<Sphere>& getSpheres();
		
		/** \brief Calculates the current sphere forces. */
		void calculateForces();
		
		/** \brief Calculates one step of the sphere movements. */
		void moveStep();
		
		/** \brief Updates local data about spheres. */
		void updateData();
		
	public:
		SphereCalculator();
		
		/**
		 * \brief Calculates the sphere movements for one step.
		 * \return Millisecond time used to calculate the step.
		 */
		unsigned int doStep();
		
		/**
		 * \brief Sets the time step.
		 * \param timeSt Requested time step in seconds.
		 */
		void setTimeStep(Scalar timeSt);
		
		/**
		 * \brief Gets the time step.
		 * \return Requested time step in seconds.
		 */
		Scalar getTimeStep();
		
		friend class SphereManager;
	};
	
}

#endif
