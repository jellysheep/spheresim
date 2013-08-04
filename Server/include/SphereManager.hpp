/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SPHEREMANAGER_HPP_
#define _SPHEREMANAGER_HPP_

#include <Sphere.hpp>
#include <SphereCalculator.hpp>

#include <QVector>

namespace SphereSim{
	
	/**
	 * \brief Manages sphere data.
	 */
	class SphereManager{
	private:
		/** \brief Provides physical calculations for the spheres. */
		SphereCalculator sphCalc;
		
		/** \brief Holds the spheres managed by the server. */
		QVector<Sphere>& spheres;
		
		/**
		 * \brief Gets the SphereCalculator.
		 * \return Requested SphereCalculator.
		 */
		SphereCalculator& getSphereCalculator();
		
	public:
		SphereManager();
		
		/**
		 * \brief Adds a specific sphere.
		 * \param s Sphere to add.
		 * \return Current sphere count.
		 */
		quint16 addSphere(Sphere s);
		/**
		 * \brief Adds a new sphere.
		 * \return Current sphere count.
		 */
		quint16 addSphere();
		
		/**
		 * \brief Removes the last sphere.
		 * \return Current sphere count.
		 */
		quint16 removeLastSphere();
		/**
		 * \brief Removes a specific sphere.
		 * \param i Index of the sphere to remove.
		 * \return Current sphere count.
		 */
		quint16 removeSphere(quint16 i);
		
		/**
		 * \brief Returns the current sphere count.
		 * \return Current sphere count.
		 */
		quint16 getCount();
		
		/**
		 * \brief Updates a specific sphere.
		 * \param i Index of the sphere to update.
		 * \param s Sphere data to update.
		 * \return Current sphere count.
		 */
		quint16 updateSphere(quint16 i, Sphere s);
		
		/**
		 * \brief Gets a specific sphere.
		 * \param i Index of the sphere to get.
		 * \return Copy of the requested sphere.
		 */
		Sphere getSphere(quint16 i);
		
		/**
		 * \brief Calculates the sphere movements for one step.
		 * \return Millisecond time used to calculate the step.
		 */
		quint16 calculateStep();
		
		friend class ActionReceiver;
	};
	
}

#endif
