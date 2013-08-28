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
#include <Integrators.hpp>
#include <ButcherTableau.hpp>

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
		quint16 sphCount;
		/** \brief Holds the current box size. */
		Vector3 boxSize;
		/** \brief Holds the current step length (time in s). */
		Scalar timeStep;
		
		/**
		 * \brief Gets the spheres managed by the server.
		 * \return Spheres managed by the server.
		 */
		QVector<Sphere>& getSpheres();
		
		/**
		 * \brief Calculates the current sphere acceleration.
		 * \param sphereIndex Index of the sphere to be calculated.
		 * \param sphere Sphere to be calculated.
		 * \return Calculated current acceleration of the sphere.
		 */
		Vector3 sphereAcceleration(quint16 sphereIndex, Sphere sphere);
		
		/** \brief Updates local data about spheres. */
		void updateData();
		
		/** \brief Stores which method is used to solve the differential equations. */
		quint8 integratorMethod;
		
		/** \brief Integrates one step of the differential equations using the Runge Kutta method defined by the Butcher tableau. */
		void integrateRungeKuttaStep();
		
		/** \brief Stores the Butcher tableau used in the integrator. */
		ButcherTableau butcherTableau;
		
	public:
		SphereCalculator();
		
		/**
		 * \brief Calculates the sphere movements for one step.
		 * \return Millisecond time used to calculate the step.
		 */
		quint16 doStep();
		
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
		
		/**
		 * \brief Sets the integrator method.
		 * \param integrMethod Requested integrator method.
		 */
		void setIntegratorMethod(quint8 integrMethod);
		
		/**
		 * \brief Gets the integrator method.
		 * \return Requested integrator method.
		 */
		quint8 getIntegratorMethod();
		
		friend class SphereManager;
	};
	
}

#endif
