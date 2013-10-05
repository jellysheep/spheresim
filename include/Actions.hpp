/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _ACTIONS_HPP_
#define _ACTIONS_HPP_

namespace SphereSim{
	
	/**
	 * \brief Lists action groups provided by the server and requested by the clients;
	 * actions are used for TCP communication between clients and server.
	 */
	namespace ActionGroups{
		enum Group{
			basic = 1,
			spheresUpdating,
			calculation,
			information,
			physicalConstants
		};
	}
	
	/**
	 * \brief Basic actions concerning client and server software.
	 */
	namespace BasicActions{
		enum Action{
			getVersion = 1,
			getTrueString,
			terminateServer,
			getFloatingType
		};
	}
	
	/**
	 * \brief Actions that update the spheres.
	 */
	namespace SpheresUpdatingActions{
		enum Action{
			addOne = 1,
			removeLast,
			updateOne,
			getCount,
			getOne,
			getOneFull
		};
	}
	
	/**
	 * \brief Actions that calculate the physical movements.
	 */
	namespace CalculationActions{
		enum Action{
			doOneStep = 1,
			setTimeStep,
			getTimeStep,
			setIntegratorMethod,
			getIntegratorMethod,
			popCalculationCounter,
			doSomeSteps,
			startSimulation,
			stopSimulation,
			getIsSimulating
		};
	}
	
	/**
	 * \brief Actions that inform about sphere properties.
	 */
	namespace InformationActions{
		enum Action{
			getTotalEnergy = 1
		};
	}
	
	/**
	 * \brief Actions that read or update physical constants.
	 */
	namespace PhysicalConstantsActions{
		enum Action{
			setSphereE = 1,
			setSpherePoisson,
			setWallE,
			setWallPoisson,
			setEarthGravity
		};
	}
	
	/**
	 * \brief Server status replies.
	 */
	namespace ServerStatusReplies{
		enum Replies{
			acknowledge = 1,
			unknownActionGroup,
			unknownAction,
			sendSphere,
			terminating
		};
	}
	
}

#endif
