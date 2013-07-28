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

namespace SphereSim{
	
	/**
	 * \brief Manages sphere data.
	 */
	class SphereManager{
	private:
		
	public:
		/** \brief Holds the spheres managed by the server. */
		Sphere* spheres;
	};
	
}

#endif
