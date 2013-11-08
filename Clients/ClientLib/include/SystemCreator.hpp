/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SYSTEMCREATOR_HPP_
#define _SYSTEMCREATOR_HPP_

#include <Vector.hpp>

class ActionSender;

namespace SphereSim
{
	
	/** \brief Create simulation systems. */
	class SystemCreator
	{
	private:
		ActionSender* actionSender;
		
	public:
		SystemCreator(ActionSender* actionSender);
		
		Scalar createArgonGasSystem(quint16 sphereCount, Scalar targetTemperature=473.15);
		
		Scalar createMacroscopicGravitationSystem(quint16 sphereCount);
		
		Scalar createMacroscopic2DCollisionSystem(quint16 sphereCount);
	};
	
}

#endif
