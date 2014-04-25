/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SPHERECALCULATOR_HPP_
#define _SPHERECALCULATOR_HPP_

#include "Sphere.hpp"
#include "Integrators.hpp"
#include "ButcherTableau.hpp"
#include "SimulatedSystem.hpp"
#include "TwoDimArray.hpp"

#include <QMutex>
#include <QObject>
#include <string>

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
        unsigned int calculationCounter;

        /** \brief Number of calculated steps. */
        unsigned int stepCounter;

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

        const unsigned short cellCount;

        const unsigned int cellCount3;

        const unsigned short maxSpheresPerCell;

        const unsigned short maxCellsPerSphere;

        TwoDimArray<unsigned short> sphereIndicesInCells;

        TwoDimArray<unsigned int> cellIndicesOfSpheres;

        const unsigned short maxCollidingSpheresPerSphere;

        TwoDimArray<unsigned short> collidingSpheresPerSphere;

        const unsigned short gravityCellCount;

        const unsigned int gravityCellCount3;

        const unsigned int gravityAllCellCount;

        const unsigned short maxSpheresPerGravityCell;

        TwoDimArray<unsigned short> sphereIndicesInGravityCells;

        Vector3 *massVectorSumPerCell;

        Scalar *massSumPerCell;

        Vector3 *massCenterPerCell;

        Vector3 *gravityCellSizes;

        Scalar *gravityCellHalfDiagonalLength;

        Vector3 *gravityCellPositions;

        const unsigned short maxApproximatingCellsPerGravityCell;

        TwoDimArray<unsigned int> approximatingCellsPerGravityCell;

        TwoDimArray<Vector3> approximatingCellsOffsetPerGravityCell;

        const unsigned short maxPairwiseCellsPerGravityCell;

        TwoDimArray<unsigned int> pairwiseCellsPerGravityCell;

        unsigned int *gravityCellIndexOfSpheres;

        unsigned short *sphereCountPerGravityCell;

        unsigned int lastStepCalculationTime;

        QElapsedTimer* elapsedTimer;

        const unsigned int &sphereCount;
        const Scalar &timeStep;
        const unsigned int &integratorMethod;
        const bool &collisionDetection;
        const bool &gravityCalculation;
        const bool &lennardJonesPotential;
        const unsigned int &maximumStepDivision;
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
        Vector3 sphereAcceleration(unsigned short sphereIndex, Sphere sphere,
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
        unsigned int integrateRungeKuttaStep_internal(unsigned short sphereIndex,
            Scalar stepLength, Scalar timeDiff, unsigned short stepDivisionCounter);

        /** \brief Stop the worker. */
        void stopWorker();

        /** \brief Remove a specific sphere.
         * \param i Index of the sphere to remove.
         * \return Current sphere count. */
        unsigned short removeSphere(unsigned short i);

        /** \brief Prepare sphere data for a frame. */
        void prepareFrameData();

        void updateSphereBox();

        void updateSphereCellLists();

        template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
            bool periodicBoundaries>
        Scalar getTotalEnergy_internal();

        void buildGravityCells();

        void rebuildGravityCellPairs();

        void rebuildGravityCellPairs(unsigned int currentCellIndex,
            unsigned int testCellIndex);

        void updateGravityCellIndexOfSpheresArray();

        void updateSphereGravityCellLists();

        void updateGravityCellData();

        void updateIntegratorMethod();

        void updateTargetTemperature();

        void updateSphereSphereE();

        void updateSphereWallE();

        unsigned short getAndUpdateSphereCount();

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
        void frameToSend(std::string frameData);

    public slots:
        /** \copydoc SpheresUpdatingActions::addSphere
         * \return New sphere count. */
        unsigned short addSphere();

        /** \copydoc SpheresUpdatingActions::removeLastSphere
         * \return Current sphere count. */
        unsigned short removeLastSphere();

        /** \copydoc SpheresUpdatingActions::updateSphere
         * \param i Index of the sphere to update.
         * \param s Sphere data to update.
         * \return Current sphere count.
         */
        unsigned short updateSphere(unsigned short i, Sphere s);

        /** \copydoc SpheresUpdatingActions::getAllSphereData
         * \param i Index of the sphere to get.
         * \return Copy of the requested sphere. */
        Sphere getAllSphereData(unsigned short i);

        /** \copydoc SpheresUpdatingActions::addSomeSpheres
         * \copydetails getSphereCount */
        unsigned short addSomeSpheres(unsigned short sphCount);

        /** \copydoc SpheresUpdatingActions::removeSomeLastSpheres
         * \copydetails getSphereCount */
        unsigned short removeSomeLastSpheres(unsigned short sphCount);

        /** \copydoc SpheresUpdatingActions::updateSpherePositionsInBox */
        void updateSpherePositionsInBox(Scalar randomDisplacement,
            Scalar randomSpeed);

        /** \copydoc SpheresUpdatingActions::updateAllSpheres
         * \param s Sphere data to update.
         * \return Current sphere count.
         */
        unsigned short updateAllSpheres(Sphere s);

        /** \copydoc SpheresUpdatingActions::updateKineticEnergy
         * \param factor Kinetic energy scale factor. */
        void updateKineticEnergy(Scalar factor);

        /** \copydoc CalculationActions::calculateStep */
        void calculateStep();

        /** \copydoc CalculationActions::popCalculationCounter
         * \return Requested number of force calculations per sphere. */
        unsigned int popCalculationCounter();

        /** \copydoc CalculationActions::calculateSomeSteps
         * \param steps Number of steps to calculate (0 = unlimited). */
        void calculateSomeSteps(unsigned int steps);

        /** \copydoc CalculationActions::startSimulation */
        void startSimulation();

        /** \copydoc CalculationActions::stopSimulation */
        void stopSimulation();

        /** \copydoc CalculationActions::popStepCounter
         * \return Requested number of calculated steps. */
        unsigned int popStepCounter();

        /** \copydoc CalculationActions::getLastStepCalculationTime */
        unsigned int getLastStepCalculationTime();

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
