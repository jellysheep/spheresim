/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONS_HPP_
#define _ACTIONS_HPP_

namespace SphereSim
{

	/** \brief Variables which are synchronized between client and server. */
	namespace SimulationVariables
	{
		/** \see SimulationVariables */
		enum Variable
		{
			/** \brief Software version of the server. */
			serverVersion,
			/** \brief Floating type of the server ("float" or "qreal"). */
			serverFloatingType,
			/** \brief Number of spheres in the simulation. */
			sphereCount,
			/** \brief Integration time step. */
			timeStep,
			/** \brief Runge-Kutta method used for integrating the system. */
			integratorMethod,
			/** \brief Flag if a simulation is currently running. */
			simulating,
			/** \brief Flag if Server sends frames regularly. */
			frameSending,
			/** \brief Flag if sphere collisions get detected. */
			collisionDetection,
			/** \brief Flag if sphere-sphere gravitational forces get calculated. */
			gravityCalculation,
			/** \brief Flag if Lennard-Jones potential and forces get calculated. */
			lennardJonesPotential,
			/** \brief Maximum number of dividing simulation steps. */
			maximumStepDivision,
			/** \brief Maximum allowed relative error for simulation steps. */
			maximumStepError,
			/** \brief Elastic modulus of spheres used for collisions. */
			sphereE,
			/** \brief Poisson ratio of spheres used for collisions. */
			spherePoissonRatio,
			/** \brief Elastic modulus of walls used for collisions. */
			wallE,
			/** \brief Poisson ratio of walls used for collisions. */
			wallPoissonRatio,
			/** \brief 3D vector of the earth gravity used for the simulation. */
			earthGravity,
			/** \brief Gravitational constant G. */
			gravitationalConstant,
			/** \brief Size of the simulated system box. */
			boxSize,
			/** \brief Target temperature achieved by adjusting kinetic energy. */
			targetTemperature,
			/** \brief Flag if periodic boundary conditions are enabled. */
			periodicBoundaryConditions,
			/** \brief Maximum theta used for Barnes-Hut algorithm. */
			maximumTheta,
			/** \brief k_Boltzmann constant. */
			kBoltzmann,
			/** \brief Epsilon used for Lennard-Jones potential. */
			lenJonPotEpsilon,
			/** \brief Sigma used for Lennard-Jones potential. */
			lenJonPotSigma,
			/** \brief Enum value indicating if all variables have been received. */
			allVariablesReceived,
			/** \brief Last enum value equals number of variables. */
			numberOfVariables
		};
	}


	/** \brief Action groups provided by the server and requested by the clients;
	 * actions are used for TCP communication between clients and server. */
	namespace ActionGroups
	{
		/** \see ActionGroups */
		enum Group
		{
			/** \copydoc BasicActions
			 * \see BasicActions */
			basic,
			/** \copydoc SpheresUpdatingActions
			 * \see SpheresUpdatingActions */
			spheresUpdating,
			/** \copydoc CalculationActions
			 * \see CalculationActions */
			calculation,
			/** \copydoc InformationActions
			 * \see InformationActions */
			information,
			/** \copydoc SimulatedSystemActions
			 * \see SimulatedSystemActions */
			simulatedSystem,
			/** \copydoc WorkQueueActions
			 * \see WorkQueueActions */
			workQueue
		};
	}

	/** \brief Basic things concerning client and server software. */
	namespace BasicActions
	{
		/** \see BasicActions */
		enum Action
		{
			/** \brief Terminate the server. */
			terminateServer,
			/** \brief Inform about an updated variable. */
			updateVariable
		};
	}

	/** \brief Update or get spheres. */
	namespace SpheresUpdatingActions
	{
		/** \see SpheresUpdatingActions */
		enum Action
		{
			/** \brief Add one sphere to the simulation, without specific data. */
			addSphere,
			/** \brief Remove the last sphere of the simulation. */
			removeLastSphere,
			/** \brief Update the data of one sphere. */
			updateSphere,
			/** \brief Get basic data (radius and position) of one sphere. */
			getBasicSphereData,
			/** \brief Get all data of one sphere. */
			getAllSphereData,
			/** \brief Add some spheres to the simulation, without specific data. */
			addSomeSpheres,
			/** \brief Remove some last spheres of the simulation. */
			removeSomeLastSpheres,
			/** \brief Update sphere positions to be distributed in simulated box.
			 * Spheres get random displacement and speed relative to their radius. */
			updateSpherePositionsInBox,
			/** \brief Update the data of all spheres. */
			updateAllSpheres,
			/** \brief Update kinetic energy of each sphere by a scale factor. */
			updateKineticEnergy
		};
	}

	/** \brief Update and get calculation parameters or control a simulation. */
	namespace CalculationActions
	{
		/** \see CalculationActions */
		enum Action
		{
			/** \brief Start a simulation of one step. */
			calculateStep,
			/** \brief Get and reset the number of force calculations per sphere. */
			popCalculationCounter,
			/** \brief Start a simulation of a specific number of steps. */
			calculateSomeSteps,
			/** \brief Start a continuous simulation (unlimited steps). */
			startSimulation,
			/** \brief Stop a currently running simulation. */
			stopSimulation,
			/** \brief Get and reset the number of simulated time steps. */
			popStepCounter,
			/** \brief Get time (in ms) needed to calculate the last step. */
			getLastStepCalculationTime
		};
	}

	/** \brief Get information about the simulation. */
	namespace InformationActions
	{
		/** \see InformationActions */
		enum Action
		{
			/** \brief Calculate and get the total energy. */
			getTotalEnergy,
			/** \brief Calculate and get the total kinetic energy. */
			getKineticEnergy
		};
	}

	/** \brief Update or get physical constants and other parameters. */
	namespace SimulatedSystemActions
	{
		/** \see SimulatedSystemActions */
		enum Action
		{
		};
	}

	/** \brief Server status replies. */
	namespace ServerStatusReplies
	{
		/** \see ServerStatusReplies */
		enum Reply
		{
			/** \brief The server status is ok. */
			acknowledge,
			/** \brief The action group requested by client is unknown to server. */
			unknownActionGroup,
			/** \brief The action requested by client is unknown to server. */
			unknownAction,
			/** \brief The server is sending sphere data. */
			sendFrame,
			/** \brief The server is terminating. */
			terminating,
			/** \brief The server is sending an updated variable. */
			sendVariable
		};
	}

	/** \brief Type of work to be done by the SimulationWorker. */
	namespace WorkQueueActions
	{
		/** \see WorkQueueAction */
		enum Action
		{
			/** \brief Stop the worker. */
			stopWorker,
			/** \brief Prepare frame data which will be sent to client. */
			prepareFrameData,
			/** \copybrief CalculationActions::calculateStep */
			calculateStep
		};
	}

}

#endif
