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
		
		/** \brief Calculates the current sphere forces. */
		void calculateForces();
		
		/** \brief Updates local data about spheres. */
		void updateData();
		
		/** \brief Stores which method is used to solve the differential equations. */
		quint8 integratorMethod;
		
		/** \brief Integrates one step of the differential equations using the Euler Cauchy method. */
		void doEulerCauchyStep();
		
		/** \brief Integrates one step of the differential equations using the Midpoint method. */
		void doMidpointStep();
		
		/** \brief Integrates one step of the differential equations using the Runge Kutta method (RK4). */
		void doRungeKutta4Step();
		
		/** \brief Integrates one step of the differential equations using the leapfrog method. */
		void doLeapfrogStep();
		
		/** \brief Integrates one step of the differential equations using the Semi-implicit Euler method. */
		void doSemiImplicitEulerStep();
		
		/** \brief Stores the method used to calculate one step of the sphere movements. */
		void (SphereCalculator::*stepMethod)();
		
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
