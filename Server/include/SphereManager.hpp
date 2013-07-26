
#ifndef _SPHEREMANAGER_HPP_
#define _SPHEREMANAGER_HPP_

#include <Sphere.hpp>

namespace SphereSim{
	
	/*
	 * class SphereManager:
	 * Manages sphere data.
	 */
	class SphereManager{
	private:
		
	public:
		/* field spheres:
		 * Holds the spheres managed by the server. */
		Sphere* spheres;
	};
	
}

#endif
