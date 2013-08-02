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

#include <QVector>

namespace SphereSim{
	
	/**
	 * \brief Manages sphere data.
	 */
	class SphereManager{
	private:
		/** \brief Holds the spheres managed by the server. */
		QVector<Sphere> spheres;
		
	public:
		SphereManager();
		
		/**
		 * \brief Adds a specific sphere.
		 * \return Current sphere count.
		 */
		unsigned int addSphere(Sphere s);
		/**
		 * \brief Adds a new sphere.
		 * \return Current sphere count.
		 */
		unsigned int addSphere();
		
		/**
		 * \brief Removes the last sphere.
		 * \return Current sphere count.
		 */
		unsigned int removeLastSphere();
		/**
		 * \brief Removes a specific sphere.
		 * \return Current sphere count.
		 */
		unsigned int removeSphere(unsigned int i);
		
		/**
		 * \brief Returns the current sphere count.
		 * \return Current sphere count.
		 */
		unsigned int getCount();
	};
	
}

#endif
