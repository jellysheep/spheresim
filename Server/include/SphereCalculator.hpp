/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SPHERECALCULATOR_HPP_
#define _SPHERECALCULATOR_HPP_

#include <Sphere.hpp>
#include <Integrators.hpp>
#include <ButcherTableau.hpp>
#include <SimulatedSystem.hpp>
#include <TwoDimArray.hpp>

#include <QMutex>
#include <QObject>

class QTimer;
class QElapsedTimer;

namespace SphereSim
{

	class SimulationWorker;
	class WorkQueue;
	class ActionReceiver;

	/** \brief Calculator of sphere movements.
	 */
	class SphereCalculator : public QObject
	{
		Q_OBJECT

	private:
		/** \brief Spheres managed by the server. */
		std::vector<Sphere> spheres;

		/** \brief New calculated positions of the spheres. */
		std::vector<Vector3> newSpherePos;

		/** \brief Butcher tableau used in the integrator. */
		ButcherTableau butcherTableau;

		/** \brief Number of calculation steps needed for simulation. */
		quint32 calculationCounter;

		/** \brief Number of calculated steps. */
		quint32 stepCounter;

		/** \brief Storage for physical constants and other variables. */
		SimulatedSystem* simulatedSystem;

		/** \brief Thread used for simulation. */
		QThread* simulationThread;

		/** \brief Mutex locking the queue status. */
		QMutex* workQueueMutex;

		/** \brief Queue storing outstanding simulation work. */
		WorkQueue* workQueue;

		/** \brief Worker used for simulation. */
		SimulationWorker* simulationWorker;

		Vector3 sphereBoxSize;

		Vector3 sphereBoxPosition;

		const quint16 cellCount;

		const quint32 cellCount3;

		const quint16 maxSpheresPerCell;

		const quint16 maxCellsPerSphere;

		TwoDimArray<quint16, true> sphereIndicesInCells;

		TwoDimArray<quint32, true> cellIndicesOfSpheres;

		const quint16 maxCollidingSpheresPerSphere;

		TwoDimArray<quint16, true> collidingSpheresPerSphere;

		const quint16 gravityCellCount;

		const quint32 gravityCellCount3;

		const quint32 gravityAllCellCount;

		const quint16 maxSpheresPerGravityCell;

		TwoDimArray<quint16, true> sphereIndicesInGravityCells;

		Vector3 *massVectorSumPerCell;

		Scalar *massSumPerCell;

		Vector3 *massCenterPerCell;

		Vector3 *gravityCellSizes;

		Scalar *gravityCellHalfDiagonalLength;

		Vector3 *gravityCellPositions;

		const quint16 maxApproximatingCellsPerGravityCell;

		TwoDimArray<quint32, true> approximatingCellsPerGravityCell;

		TwoDimArray<Vector3, true> approximatingCellsOffsetPerGravityCell;

		const quint16 maxPairwiseCellsPerGravityCell;

		TwoDimArray<quint32, true> pairwiseCellsPerGravityCell;

		quint32 *gravityCellIndexOfSpheres;

		quint16 *sphereCountPerGravityCell;

		quint32 lastStepCalculationTime;

		QElapsedTimer* elapsedTimer;

		const int &sphereCount;
		const Scalar &timeStep;
		const int &integratorMethod;
		const bool &collisionDetection;
		const bool &gravityCalculation;
		const bool &lennardJonesPotential;
		const int &maximumStepDivision;
		const Scalar &maximumStepError;
		const Scalar &sphereE;
		const Scalar &spherePoissonRatio;
		const Scalar &wallE;
		const Scalar &wallPoissonRatio;
		const Vector3 &earthGravity;
		const Scalar &gravitationalConstant;
		const Vector3 &boxSize;
		const Scalar &targetTemperature;
		const bool &periodicBoundaryConditions;
		const Scalar &maximumTheta;
		const Scalar &kBoltzmann;
		const Scalar &lenJonPotEpsilon;
		const Scalar &lenJonPotSigma;

		Scalar sphereSphereE;
		Scalar sphereWallE;

		/** \brief Calculate the current sphere acceleration.
		 * \param sphereIndex Index of the sphere to be calculated.
		 * \param sphere Sphere to be calculated.
		 * \param timeDiff Time difference (in s) for movements of other spheres.
		 * \return Calculated current acceleration of the sphere. */
		template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
			bool periodicBoundaries>
		Vector3 sphereAcceleration(quint16 sphereIndex, Sphere sphere,
			Scalar timeDiff);

		/** \brief Integrate one step using specified Runge Kutta method. */
		void integrateRungeKuttaStep();

		/** \copydoc integrateRungeKuttaStep */
		template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
			bool periodicBoundaries>
		void integrateRungeKuttaStep_internal();

		/** \brief Integrates one step of one sphere.
		 * \param sphereIndex Index of the sphere to be integrated.
		 * \param stepLength Current step length (time in s).
		 * \param timeDiff Time difference (in s) for movements of other spheres.
		 * \return Number of steps used to integrate. */
		template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
			bool periodicBoundaries>
		quint32 integrateRungeKuttaStep_internal(quint16 sphereIndex,
			Scalar stepLength, Scalar timeDiff, quint16 stepDivisionCounter);

		/** \brief Stop the worker. */
		void stopWorker();

		/** \brief Remove a specific sphere.
		 * \param i Index of the sphere to remove.
		 * \return Current sphere count. */
		quint16 removeSphere(quint16 i);

		/** \brief Prepare sphere data for a frame. */
		void prepareFrameData();

		void updateSphereBox();

		void updateSphereCellLists();

		template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
			bool periodicBoundaries>
		Scalar getTotalEnergy_internal();

		void buildGravityCells();

		void rebuildGravityCellPairs();

		void rebuildGravityCellPairs(quint32 currentCellIndex,
			quint32 testCellIndex);

		void updateGravityCellIndexOfSpheresArray();

		void updateSphereGravityCellLists();

		void updateGravityCellData();

		void updateIntegratorMethod();

		void updateTargetTemperature();

		void updateSphereSphereE();

		void updateSphereWallE();

		quint16 getAndUpdateSphereCount();

	public:
		SphereCalculator(ActionReceiver* actRcv, SimulatedSystem* simulatedSystem);
		~SphereCalculator();

		SphereCalculator() = delete;
		SphereCalculator(const SphereCalculator&) = delete;
		SphereCalculator& operator=(const SphereCalculator&) = delete;

		WorkQueue* getWorkQueue();

		friend class SimulationWorker;

	signals:
		/** \brief Stop the running simulation. */
		void requestingSimulationStop();

		/** \brief Stop and delete the worker. */
		void requestingWorkerStop();

		/** \brief Send frame to client. */
		void frameToSend(QByteArray frameData);

	public slots:
		/** \copydoc SpheresUpdatingActions::addSphere
		 * \return New sphere count. */
		quint16 addSphere();

		/** \copydoc SpheresUpdatingActions::removeLastSphere
		 * \return Current sphere count. */
		quint16 removeLastSphere();

		/** \copydoc SpheresUpdatingActions::updateSphere
		 * \param i Index of the sphere to update.
		 * \param s Sphere data to update.
		 * \return Current sphere count.
		 */
		quint16 updateSphere(quint16 i, Sphere s);

		/** \copydoc SpheresUpdatingActions::getAllSphereData
		 * \param i Index of the sphere to get.
		 * \return Copy of the requested sphere. */
		Sphere getAllSphereData(quint16 i);

		/** \copydoc SpheresUpdatingActions::addSomeSpheres
		 * \copydetails getSphereCount */
		quint16 addSomeSpheres(quint16 sphCount);

		/** \copydoc SpheresUpdatingActions::removeSomeLastSpheres
		 * \copydetails getSphereCount */
		quint16 removeSomeLastSpheres(quint16 sphCount);

		/** \copydoc SpheresUpdatingActions::updateSpherePositionsInBox */
		void updateSpherePositionsInBox(Scalar randomDisplacement,
			Scalar randomSpeed);

		/** \copydoc SpheresUpdatingActions::updateAllSpheres
		 * \param s Sphere data to update.
		 * \return Current sphere count.
		 */
		quint16 updateAllSpheres(Sphere s);

		/** \copydoc SpheresUpdatingActions::updateKineticEnergy
		 * \param factor Kinetic energy scale factor. */
		void updateKineticEnergy(Scalar factor);

		/** \copydoc CalculationActions::calculateStep */
		void calculateStep();

		/** \copydoc CalculationActions::popCalculationCounter
		 * \return Requested number of force calculations per sphere. */
		quint32 popCalculationCounter();

		/** \copydoc CalculationActions::calculateSomeSteps
		 * \param steps Number of steps to calculate (0 = unlimited). */
		void calculateSomeSteps(quint32 steps);

		/** \copydoc CalculationActions::startSimulation */
		void startSimulation();

		/** \copydoc CalculationActions::stopSimulation */
		void stopSimulation();

		/** \copydoc CalculationActions::popStepCounter
		 * \return Requested number of calculated steps. */
		quint32 popStepCounter();

		/** \copydoc CalculationActions::getLastStepCalculationTime */
		quint32 getLastStepCalculationTime();

		/** \copydoc InformationActions::getTotalEnergy
		 * \return Requested total energy. */
		Scalar getTotalEnergy();

		/** \copydoc InformationActions::getKineticEnergy
		 * \return Requested kinetic energy. */
		Scalar getKineticEnergy();

		void variableUpdated(int var);

	};

}

#endif
