/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONS_HPP_
#define _ACTIONS_HPP_

namespace SphereSim
{
	
	/** \brief Action groups provided by the server and requested by the clients;
	 * actions are used for TCP communication between clients and server. */
	namespace ActionGroups
	{
		/** \see ActionGroups */
		enum Group
		{
			/** \copydoc BasicActions
			 * \see BasicActions */
			basic = 1,
			/** \copydoc SpheresUpdatingActions
			 * \see SpheresUpdatingActions */
			spheresUpdating,
			/** \copydoc CalculationActions
			 * \see CalculationActions */
			calculation,
			/** \copydoc InformationActions
			 * \see InformationActions */
			information,
			/** \copydoc PhysicalConstantsActions
			 * \see PhysicalConstantsActions */
			physicalConstants
		};
	}
	
	/** \brief Basic things concerning client and server software. */
	namespace BasicActions
	{
		/** \see BasicActions */
		enum Action
		{
			/** \brief Get the software version of the server. */
			getServerVersion = 1,
			/** \brief Get a string "true". */
			getTrueString,
			/** \brief Terminate the server. */
			terminateServer,
			/** \brief Get the floating type of the server ("float" or "qreal"). */
			getServerFloatingType
		};
	}
	
	/** \brief Update or get spheres. */
	namespace SpheresUpdatingActions
	{
		/** \see SpheresUpdatingActions */
		enum Action
		{
			/** \brief Add one sphere to the simulation, without specific data. */
			addSphere = 1,
			/** \brief Remove the last sphere of the simulation. */
			removeLastSphere,
			/** \brief Update the data of one sphere. */
			updateSphere,
			/** \brief Get the number of spheres in the simulation. */
			getSphereCount,
			/** \brief Get basic data (radius and position) of one sphere. */
			getBasicSphereData,
			/** \brief Get all data of one sphere. */
			getAllSphereData
		};
	}
	
	/** \brief Update and get calculation parameters or control a simulation. */
	namespace CalculationActions
	{
		/** \see CalculationActions */
		enum Action
		{
			/** \brief Start a simulation of one step. */
			calculateStep = 1,
			/** \brief Update the integration time step. */
			updateTimeStep,
			/** \brief Get the integration time step. */
			getTimeStep,
			/** \brief Update the Runge-Kutta method used for integrating the system. */
			updateIntegratorMethod,
			/** \brief Get the Runge-Kutta method used for integrating the system. */
			getIntegratorMethod,
			/** \brief Get and reset the number of force calculations per sphere. */
			popCalculationCounter,
			/** \brief Start a simulation of a specific number of steps. */
			calculateSomeSteps,
			/** \brief Start a continuous simulation (unlimited steps). */
			startSimulation,
			/** \brief Stop a currently running simulation. */
			stopSimulation,
			/** \brief Get if a simulation is currently running. */
			getIsSimulating,
			/** \brief Get and reset the number of simulated time steps. */
			popStepCounter
		};
	}
	
	/** \brief Get information about the simulation. */
	namespace InformationActions
	{
		/** \see InformationActions */
		enum Action
		{
			/** \brief Calculate and get the total energy of the simulated system. */
			getTotalEnergy = 1
		};
	}
	
	/** \brief Update or get physical constants. */
	namespace PhysicalConstantsActions
	{
		/** \see PhysicalConstantsActions */
		enum Action
		{
			/** \brief Update the elastic modulus of the sphere material used for sphere-sphere or sphere-wall collisions. */
			updateSphereE = 1,
			/** \brief Update the Poisson ratio of the sphere material used for sphere-sphere or sphere-wall collisions. */
			updateSpherePoissonRatio,
			/** \brief Update the elastic modulus of the wall material used for sphere-wall collisions. */
			updateWallE,
			/** \brief Update the Poisson ratio of the wall material used for sphere-wall collisions. */
			updateWallPoissonRatio,
			/** \brief Update the 3D vector of the earth gravity used for the simulation. */
			updateEarthGravity
		};
	}
	
	/** \brief Server status replies. */
	namespace ServerStatusReplies
	{
		/** \see ServerStatusReplies */
		enum Replies
		{
			/** \brief The server status is ok. */
			acknowledge = 1,
			/** \brief The action group requested by the client is unknown to the server. */
			unknownActionGroup,
			/** \brief The action requested by the client is unknown to the server. */
			unknownAction,
			/** \brief The server is sending sphere data. */
			sendFrame,
			/** \brief The server is terminating. */
			terminating
		};
	}
	
}

#endif
