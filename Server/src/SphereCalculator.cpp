/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <SphereCalculator.hpp>
#include <SimulationWorker.hpp>
#include <WorkQueue.hpp>
#include <Console.hpp>
#include <SphereTransmit.hpp>
#include <ActionReceiver.hpp>

#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <cmath>
#include <random>
#include <chrono>

#ifndef NO_OPENMP
    #define NO_OPENMP 0
#endif
#if NO_OPENMP != 1
    #include <omp.h>
#endif

using namespace SphereSim;

SphereCalculator::SphereCalculator(ActionReceiver* actRcv,
    SimulatedSystem *simulatedSystem)
    :spheres(), newSpherePos(), butcherTableau(), calculationCounter(0),
    stepCounter(0), simulatedSystem(simulatedSystem),
    simulationThread(new QThread()), workQueueMutex(new QMutex()),
    workQueue(new WorkQueue(workQueueMutex,
        simulatedSystem->getRef<bool>(SimulationVariables::frameSending))),
    simulationWorker(new SimulationWorker(this, workQueue, actRcv)),
    sphereBoxSize(), sphereBoxPosition(), cellCount(8),
    cellCount3((quint32)cellCount*cellCount*cellCount),
    maxSpheresPerCell(1024), maxCellsPerSphere(1024),
    sphereIndicesInCells(maxSpheresPerCell, cellCount3),
    cellIndicesOfSpheres(maxCellsPerSphere), maxCollidingSpheresPerSphere(300),
    collidingSpheresPerSphere(maxCollidingSpheresPerSphere), gravityCellCount(4),
    gravityCellCount3(gravityCellCount*gravityCellCount*gravityCellCount),
    gravityAllCellCount(2*gravityCellCount3), maxSpheresPerGravityCell(1024),
    sphereIndicesInGravityCells(maxSpheresPerGravityCell, gravityCellCount3),
    massVectorSumPerCell(nullptr), massSumPerCell(nullptr),
    massCenterPerCell(nullptr), gravityCellSizes(nullptr),
    gravityCellHalfDiagonalLength(nullptr), gravityCellPositions(nullptr),
    maxApproximatingCellsPerGravityCell(gravityCellCount3),
    approximatingCellsPerGravityCell(maxApproximatingCellsPerGravityCell,
        gravityAllCellCount),
    approximatingCellsOffsetPerGravityCell(maxApproximatingCellsPerGravityCell,
        gravityAllCellCount),
    maxPairwiseCellsPerGravityCell(gravityCellCount3), pairwiseCellsPerGravityCell(
        maxPairwiseCellsPerGravityCell, gravityAllCellCount),
    gravityCellIndexOfSpheres(nullptr), sphereCountPerGravityCell(nullptr),
    lastStepCalculationTime(0), elapsedTimer(new QElapsedTimer()),
    sphereCount(simulatedSystem->getRef<int>(SimulationVariables::sphereCount)),
    timeStep(simulatedSystem->getRef<Scalar>(SimulationVariables::timeStep)),
    integratorMethod(simulatedSystem->getRef<int>(
        SimulationVariables::integratorMethod)),
    collisionDetection(simulatedSystem->getRef<bool>(
        SimulationVariables::collisionDetection)),
    gravityCalculation(simulatedSystem->getRef<bool>(
        SimulationVariables::gravityCalculation)),
    lennardJonesPotential(simulatedSystem->getRef<bool>(
        SimulationVariables::lennardJonesPotential)),
    maximumStepDivision(simulatedSystem->getRef<int>(
        SimulationVariables::maximumStepDivision)),
    maximumStepError(simulatedSystem->getRef<Scalar>(
        SimulationVariables::maximumStepError)),
    sphereE(simulatedSystem->getRef<Scalar>(
        SimulationVariables::sphereE)),
    spherePoissonRatio(simulatedSystem->getRef<Scalar>(
        SimulationVariables::spherePoissonRatio)),
    wallE(simulatedSystem->getRef<Scalar>(
        SimulationVariables::wallE)),
    wallPoissonRatio(simulatedSystem->getRef<Scalar>(
        SimulationVariables::wallPoissonRatio)),
    earthGravity(simulatedSystem->getRef<Vector3>(
        SimulationVariables::earthGravity)),
    gravitationalConstant(simulatedSystem->getRef<Scalar>(
        SimulationVariables::gravitationalConstant)),
    boxSize(simulatedSystem->getRef<Vector3>(
        SimulationVariables::boxSize)),
    targetTemperature(simulatedSystem->getRef<Scalar>(
        SimulationVariables::targetTemperature)),
    periodicBoundaryConditions(simulatedSystem->getRef<bool>(
        SimulationVariables::periodicBoundaryConditions)),
    maximumTheta(simulatedSystem->getRef<Scalar>(
        SimulationVariables::maximumTheta)),
    kBoltzmann(simulatedSystem->getRef<Scalar>(
        SimulationVariables::kBoltzmann)),
    lenJonPotEpsilon(simulatedSystem->getRef<Scalar>(
        SimulationVariables::lenJonPotEpsilon)),
    lenJonPotSigma(simulatedSystem->getRef<Scalar>(
        SimulationVariables::lenJonPotSigma)),
    sphereSphereE(0), sphereWallE(0)
{
    qDebug()<<"SphereCalculator: constructor called";
#if NO_OPENMP != 1
    quint16 ompThreads = 0;
    _Pragma("omp parallel")
    _Pragma("omp atomic")
    ompThreads++;
    qDebug()<<"SphereCalculator: number of OpenMP threads:"
        <<omp_get_num_threads()<<"|"<<ompThreads;
#endif
    updateSphereBox();
    massVectorSumPerCell = new Vector3[gravityAllCellCount];
    massSumPerCell = new Scalar[gravityAllCellCount];
    massCenterPerCell = new Vector3[gravityAllCellCount];
    gravityCellSizes = new Vector3[gravityAllCellCount];
    gravityCellHalfDiagonalLength = new Scalar[gravityAllCellCount];
    gravityCellPositions = new Vector3[gravityAllCellCount];
    sphereCountPerGravityCell = new quint16[gravityAllCellCount];
    updateGravityCellIndexOfSpheresArray();
    buildGravityCells();
    rebuildGravityCellPairs();
    updateIntegratorMethod();
    updateSphereSphereE();
    updateSphereWallE();
    QObject::connect(simulatedSystem, SIGNAL(variableUpdated(int)),
        SLOT(variableUpdated(int)));

    simulationWorker->moveToThread(simulationThread);
    QObject::connect(simulationThread, SIGNAL(started()),
        simulationWorker, SLOT(work()));
    QObject::connect(simulationWorker, SIGNAL(finished()),
        simulationThread, SLOT(quit()));
    QObject::connect(simulationWorker, SIGNAL(finished()),
        simulationWorker, SLOT(deleteLater()));
    QObject::connect(simulationThread, SIGNAL(finished()),
        simulationThread, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(requestingSimulationStop()),
        workQueue, SLOT(stopSimulation()));
    QObject::connect(this, SIGNAL(requestingWorkerStop()),
        simulationWorker, SLOT(stop()));
    QObject::connect(this, SIGNAL(requestingWorkerStop()),
        workQueue, SLOT(stop()));
    QObject::connect(simulationWorker, SIGNAL(sendReply(quint8, QByteArray)),
        actRcv, SLOT(sendReply(quint8, QByteArray)));
    simulationThread->start();
}

SphereCalculator::~SphereCalculator()
{
    stopWorker();
    while (simulationWorker->getHasFinished() == false)
    {
    }
    delete[] massVectorSumPerCell;
    delete[] massSumPerCell;
    delete[] massCenterPerCell;
    delete[] gravityCellSizes;
    delete[] gravityCellHalfDiagonalLength;
    delete[] gravityCellPositions;
    if (gravityCellIndexOfSpheres != nullptr)
    {
        delete[] gravityCellIndexOfSpheres;
    }
    delete[] sphereCountPerGravityCell;
    delete elapsedTimer;
}

WorkQueue* SphereCalculator::getWorkQueue()
{
    return workQueue;
}

quint16 SphereCalculator::getAndUpdateSphereCount()
{
    simulatedSystem->set<int>(SimulationVariables::sphereCount, spheres.size());
    return spheres.size();
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
    bool periodicBoundaries>
Vector3 SphereCalculator::sphereAcceleration(quint16 sphereIndex, Sphere sphere,
    Scalar timeDiff)
{
    Scalar d, forceNormalized;
    Vector3 force, acc, dVec, dNormalized;
    quint16 sphereIndex2;
    Sphere sphere2;

    force = earthGravity*sphere.mass;
    for (quint8 dim = 0; dim<3; dim++)
    {
        if ((d = (sphere.radius - sphere.pos(dim))) > 0)
        {
            forceNormalized = 4.0/3.0*sphereWallE*sqrt(sphere.radius*d*d*d);
            force(dim) += forceNormalized;
        }
        if ((d = (sphere.radius + sphere.pos(dim) - boxSize(dim))) > 0)
        {
            forceNormalized = 4.0/3.0*sphereWallE*sqrt(sphere.radius*d*d*d);
            force(dim) -= forceNormalized;
        }
    }
    if (detectCollisions)
    {
        quint32 cellIndex;
        Scalar bothRadii, dOverlapping, R;
        collidingSpheresPerSphere.resetCounter(sphereIndex);
        for (int i = cellIndicesOfSpheres.getCounter(sphereIndex)-1; i>=0; i--)
        {
            cellIndex = cellIndicesOfSpheres[sphereIndex][i];
            for (int j = sphereIndicesInCells.getCounter(cellIndex)-1; j>=0; j--)
            {
                sphereIndex2 = sphereIndicesInCells[cellIndex][j];
                if (sphereIndex2 != sphereIndex)
                {
                    if (collidingSpheresPerSphere.addElementIfNotContained(
                        sphereIndex, sphereIndex2))
                    {
                        sphere2 = spheres[sphereIndex2];
                        sphere2.pos += timeDiff*sphere2.speed;
                            // + 0.5*sphere2.acc*timeDiff*timeDiff;
                        dVec = sphere2.pos - sphere.pos;
                        d = dVec.norm();
                        bothRadii = sphere2.radius + sphere.radius;
                        if (d < bothRadii)
                        {
                            dNormalized = dVec/d;
                            dOverlapping = bothRadii - d;
                            R = 1/((1/sphere.radius)+(1/sphere2.radius));
                            forceNormalized = 4.0f/3.0f*sphereSphereE
                                *sqrt(R*dOverlapping*dOverlapping*dOverlapping);
                            force -= forceNormalized*dNormalized;
                        }
                    }
                }
            }
        }
    }

    if (gravity || lennardJonesPotential)
    {
        quint32 gravityCellIndex = gravityCellCount3
            + gravityCellIndexOfSpheres[sphereIndex];
        quint32 gravityCellIndex2;

        Vector3 sphereOffset;
        Vector3 sphereTestPos;
        Vector3 sphereTestPos2;
        Vector3 dVecNew;
        Scalar dNew;
        for (int i = pairwiseCellsPerGravityCell.getCounter(gravityCellIndex)-1;
            i>=0; i--)
        {
            gravityCellIndex2 = pairwiseCellsPerGravityCell[gravityCellIndex][i]
                - gravityCellCount3;
            for (int j = sphereIndicesInGravityCells.getCounter(gravityCellIndex2)-1;
                j>=0; j--)
            {
                sphereIndex2 = sphereIndicesInGravityCells[gravityCellIndex2][j];
                if (sphereIndex == sphereIndex2)
                {
                    continue;
                }
                sphere2 = spheres[sphereIndex2];
                sphere2.pos += timeDiff*sphere2.speed;
                    // + 0.5*sphere2.acc*timeDiff*timeDiff;
                if (periodicBoundaries)
                {
                    sphereOffset.setZero();
                    dVec = sphere2.pos-sphere.pos;
                    d = dVec.norm();
                    sphereTestPos = sphere2.pos;
                    for (quint8 dim = 0; dim<3; dim++)
                    {
                        sphereTestPos2 = sphereTestPos;
                        sphereTestPos2(dim) += boxSize(dim);
                        dVecNew = sphereTestPos2-sphere.pos;
                        dNew = dVecNew.norm();
                        if (dNew < d)
                        {
                            dVec = dVecNew;
                            d = dNew;
                            sphereTestPos = sphereTestPos2;
                        }
                        else
                        {
                            sphereTestPos2 = sphereTestPos;
                            sphereTestPos2(dim) -= boxSize(dim);
                            dVecNew = sphereTestPos2-sphere.pos;
                            dNew = dVecNew.norm();
                            if (dNew < d)
                            {
                                dVec = dVecNew;
                                d = dNew;
                                sphereTestPos = sphereTestPos2;
                            }
                        }
                    }
                }
                else
                {
                    dVec = sphere2.pos-sphere.pos;
                    d = dVec.norm();
                }
                if (gravity)
                {
                    force += gravitationalConstant * sphere.mass * sphere2.mass
                        * dVec / d / d / d;
                }
                if (lennardJonesPotential)
                {
                    force -= 48*lenJonPotEpsilon/(lenJonPotSigma*lenJonPotSigma)
                        *dVec*(pow(lenJonPotSigma/d, 14)
                        -0.5*pow(lenJonPotSigma/d, 8));
                }
            }
        }
        for (int i = approximatingCellsPerGravityCell.getCounter(gravityCellIndex)-1;
            i>=0; i--)
        {
            gravityCellIndex2 =
                approximatingCellsPerGravityCell[gravityCellIndex][i];
            if (periodicBoundaries)
            {
                dVec = (massCenterPerCell[gravityCellIndex2]
                    +approximatingCellsOffsetPerGravityCell[gravityCellIndex][i])
                    -sphere.pos;
            }
            else
            {
                dVec = massCenterPerCell[gravityCellIndex2]-sphere.pos;
            }
            d = dVec.norm();
            if (gravity)
            {
                force += gravitationalConstant * sphere.mass
                    * massSumPerCell[gravityCellIndex2] * dVec / d / d / d;
            }
            if (lennardJonesPotential)
            {
                force -= 48*lenJonPotEpsilon/(lenJonPotSigma*lenJonPotSigma)
                    *dVec*(pow(lenJonPotSigma/d, 14)-0.5*pow(lenJonPotSigma/d, 8))
                    *sphereCountPerGravityCell[gravityCellIndex2];
            }
        }
    }

    acc = force/sphere.mass;
    _Pragma("omp atomic")
    calculationCounter++;
    return acc;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
    bool periodicBoundaries>
Scalar SphereCalculator::getTotalEnergy_internal()
{
    if (detectCollisions || gravity || lennardJonesPotential)
    {
        updateSphereBox();
    }
    if (detectCollisions)
    {
        updateSphereCellLists();
    }
    if (gravity || lennardJonesPotential)
    {
        updateSphereGravityCellLists();
        updateGravityCellData();
    }

    Scalar totalEnergy = 0.0, sphereEnergy, d;
    Sphere sphere;
    for (quint16 sphereIndex = 0; sphereIndex<spheres.size(); ++sphereIndex)
    {
        sphere = spheres[sphereIndex];
        sphereEnergy = -sphere.mass*earthGravity.dot(sphere.pos);
        sphereEnergy += 0.5*sphere.mass*sphere.speed.squaredNorm();

        for (quint8 dim = 0; dim<3; dim++)
        {
            if ((d = (sphere.radius - sphere.pos(dim))) > 0)
            {
                sphereEnergy += 8.0/15.0*sphereWallE*sqrt(sphere.radius)*pow(d, 2.5);
            }
            if ((d = (sphere.radius + sphere.pos(dim) - boxSize(dim))) > 0)
            {
                sphereEnergy += 8.0/15.0*sphereWallE*sqrt(sphere.radius)*pow(d, 2.5);
            }
        }

        if (detectCollisions)
        {
            quint32 cellIndex;
            quint16 sphereIndex2;
            Sphere sphere2;
            Vector3 dVec;
            Scalar d, bothRadii, dOverlapping, R, energy;
            collidingSpheresPerSphere.resetCounter(sphereIndex);
            for (int i = cellIndicesOfSpheres.getCounter(sphereIndex)-1; i>=0; i--)
            {
                cellIndex = cellIndicesOfSpheres[sphereIndex][i];
                for (int j = sphereIndicesInCells.getCounter(cellIndex)-1; j>=0; j--)
                {
                    sphereIndex2 = sphereIndicesInCells[cellIndex][j];
                    if (sphereIndex2 != sphereIndex)
                    {
                        if (collidingSpheresPerSphere.addElementIfNotContained(
                            sphereIndex, sphereIndex2))
                        {
                            sphere2 = spheres[sphereIndex2];
                            dVec = sphere2.pos - sphere.pos;
                            d = dVec.norm();
                            bothRadii = sphere2.radius + sphere.radius;
                            if (d < bothRadii)
                            {
                                dOverlapping = bothRadii - d;
                                R = 1/((1/sphere.radius)+(1/sphere2.radius));
                                energy = 8.0/15.0*sphereSphereE*sqrt(R)
                                    *pow(dOverlapping, 2.5);
                                sphereEnergy += energy;
                            }
                        }
                    }
                }
            }
        }

        if (gravity || lennardJonesPotential)
        {
            quint32 gravityCellIndex = gravityCellCount3
                + gravityCellIndexOfSpheres[sphereIndex];
            quint32 gravityCellIndex2;
            quint16 sphereIndex2;
            Sphere sphere2;
            Vector3 dVec;
            for (int i = pairwiseCellsPerGravityCell.getCounter(gravityCellIndex)-1;
                i>=0; i--)
            {
                gravityCellIndex2 = pairwiseCellsPerGravityCell[gravityCellIndex][i]
                    - gravityCellCount3;
                for (int j = sphereIndicesInGravityCells.getCounter(
                    gravityCellIndex2)-1; j>=0; j--)
                {
                    sphereIndex2 = sphereIndicesInGravityCells[gravityCellIndex2][j];
                    if (sphereIndex == sphereIndex2)
                    {
                        continue;
                    }
                    sphere2 = spheres[sphereIndex2];
                    dVec = sphere2.pos-sphere.pos;
                    d = dVec.norm();
                    if (gravity)
                    {
                        sphereEnergy -= gravitationalConstant * sphere.mass *
                            sphere2.mass / d;
                    }
                    if (lennardJonesPotential)
                    {
                        sphereEnergy += 4*lenJonPotEpsilon
                            *(pow(lenJonPotSigma/d, 12)-pow(lenJonPotSigma/d, 6));
                    }
                }
            }
            for (int i = approximatingCellsPerGravityCell.getCounter(
                gravityCellIndex)-1; i>=0; i--)
            {
                gravityCellIndex2 =
                    approximatingCellsPerGravityCell[gravityCellIndex][i];
                if (periodicBoundaries)
                {
                    dVec = (massCenterPerCell[gravityCellIndex2]
                        +approximatingCellsOffsetPerGravityCell[gravityCellIndex][i])
                        -sphere.pos;
                }
                else
                {
                    dVec = massCenterPerCell[gravityCellIndex2]-sphere.pos;
                }
                d = dVec.norm();
                if (gravity)
                {
                    sphereEnergy -= gravitationalConstant * sphere.mass *
                        massSumPerCell[gravityCellIndex2] / d;
                }
                if (lennardJonesPotential)
                {
                    sphereEnergy += 4*lenJonPotEpsilon
                        *(pow(lenJonPotSigma/d, 12)-pow(lenJonPotSigma/d, 6))
                        *sphereCountPerGravityCell[gravityCellIndex2];
                }
            }
        }

        totalEnergy += sphereEnergy;
    }
    return totalEnergy;
}

void SphereCalculator::integrateRungeKuttaStep()
{
    elapsedTimer->start();
    if (collisionDetection)
    {
        if (gravityCalculation)
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<true, true, true, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<true, true, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<true, true, false, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<true, true, false, false>();
                }
            }
        }
        else
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<true, false, true, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<true, false, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<true, false, false, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<true, false, false, false>();
                }
            }
        }
    }
    else
    {
        if (gravityCalculation)
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<false, true, true, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<false, true, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<false, true, false, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<false, true, false, false>();
                }
            }
        }
        else
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<false, false, true, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<false, false, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    integrateRungeKuttaStep_internal<false, false, false, true>();
                }
                else
                {
                    integrateRungeKuttaStep_internal<false, false, false, false>();
                }
            }
        }
    }
    lastStepCalculationTime = elapsedTimer->elapsed()+1;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
    bool periodicBoundaries>
void SphereCalculator::integrateRungeKuttaStep_internal()
{
    if (detectCollisions || gravity || lennardJonesPotential)
    {
        updateSphereBox();
    }
    if (detectCollisions)
    {
        updateSphereCellLists();
    }
    if (gravity || lennardJonesPotential)
    {
        updateSphereGravityCellLists();
        updateGravityCellData();
    }
    _Pragma("omp parallel for schedule(dynamic,1)")
    for (quint16 sphereIndex = 0; sphereIndex<spheres.size(); ++sphereIndex)
    {
        newSpherePos[sphereIndex] = spheres[sphereIndex].pos;
        integrateRungeKuttaStep_internal<detectCollisions, gravity,
            lennardJonesPotential, periodicBoundaries>(sphereIndex, timeStep,
            0.0, 0);
    }
    _Pragma("omp parallel for schedule(dynamic,1)")
    for (quint16 sphereIndex = 0; sphereIndex<spheres.size(); ++sphereIndex)
    {
        if (periodicBoundaries)
        {
            Vector3 pos = newSpherePos[sphereIndex];
            for (quint8 dim = 0; dim<3; dim++)
            {
                if (pos(dim) > boxSize(dim))
                {
                    pos(dim) -= boxSize(dim);
                }
                else if (pos(dim) < 0)
                {
                    pos(dim) += boxSize(dim);
                }
            }
            spheres[sphereIndex].pos = pos;
        }
        else
        {
            spheres[sphereIndex].pos = newSpherePos[sphereIndex];
        }
    }
    stepCounter++;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential,
    bool periodicBoundaries>
quint32 SphereCalculator::integrateRungeKuttaStep_internal(quint16 sphereIndex,
    Scalar stepLength, Scalar timeDiff, quint16 stepDivisionCounter)
{
    Sphere sphere = spheres[sphereIndex];
    sphere.pos = newSpherePos[sphereIndex];
    Sphere origSphere = sphere;
    const quint8 integratorOrder = butcherTableau.order;
    Vector3 k_acc[integratorOrder];
    Vector3 k_speed[integratorOrder];

    k_acc[0] = sphereAcceleration<detectCollisions, gravity, lennardJonesPotential,
        periodicBoundaries>(sphereIndex, sphere, timeDiff);
    k_speed[0] = sphere.speed;
    for (quint8 n = 1; n<integratorOrder; n++)
    {
        sphere.pos = origSphere.pos;
        for (quint8 j = 0; j<n; j++)
        {
            sphere.pos += stepLength*butcherTableau.a[n][j]*k_speed[j];
        }
        k_acc[n] = sphereAcceleration<detectCollisions, gravity,
            lennardJonesPotential, periodicBoundaries>(sphereIndex, sphere,
            timeDiff);

        k_speed[n] = origSphere.speed;
        for (quint8 j = 0; j<n; j++)
        {
            k_speed[n] += stepLength*butcherTableau.a[n][j]*k_acc[j];
        }
    }

    Vector3 pos = origSphere.pos;
    Vector3 pos_ = pos;
    Vector3 speed = origSphere.speed;
    Vector3 speed_ = speed;
    for (quint8 j = 0; j<integratorOrder; j++)
    {
        pos += stepLength*butcherTableau.b[j]*k_speed[j];
        pos_ += stepLength*butcherTableau.b_[j]*k_speed[j];
        speed += stepLength*butcherTableau.b[j]*k_acc[j];
        speed_ += stepLength*butcherTableau.b_[j]*k_acc[j];
    }

    if (stepDivisionCounter < maximumStepDivision)
    {
        Scalar error_pos_ = (pos-pos_).norm();
        Scalar error_speed_ = (speed-speed_).norm();
        Scalar dPos = (pos-origSphere.pos).norm();
        Scalar dSpeed = (speed-origSphere.speed).norm();
        if (error_pos_ > dPos*maximumStepError
            || error_speed_ > dSpeed*maximumStepError)
        {
            quint32 stepCount = 0;
            stepCount += integrateRungeKuttaStep_internal<detectCollisions, gravity,
                lennardJonesPotential, periodicBoundaries>(sphereIndex, stepLength/2,
                timeDiff, stepDivisionCounter+1);
            stepCount += integrateRungeKuttaStep_internal<detectCollisions, gravity,
                lennardJonesPotential, periodicBoundaries>(sphereIndex, stepLength/2,
                timeDiff+(stepLength/2), stepDivisionCounter+1);
            return stepCount;
        }
    }
    newSpherePos[sphereIndex] = pos;
    spheres[sphereIndex].speed = speed;
    spheres[sphereIndex].acc = (speed-origSphere.speed)/stepLength;
    return 1;
}

quint16 SphereCalculator::removeSphere(quint16 i)
{
    if (spheres.size()>i)
    {
        spheres.erase(spheres.begin()+i);
        newSpherePos.erase(newSpherePos.begin()+i);
        cellIndicesOfSpheres.resize(spheres.size());
        collidingSpheresPerSphere.resize(spheres.size());
        updateGravityCellIndexOfSpheresArray();
    }
    return getAndUpdateSphereCount();
}

void SphereCalculator::stopWorker()
{
    emit requestingWorkerStop();
}

void SphereCalculator::prepareFrameData()
{
    QByteArray frameData;
    QDataStream dataStream(&frameData, QIODevice::WriteOnly);
    dataStream<<(quint16)spheres.size();
    for (quint16 i = 0; i<spheres.size(); i++)
    {
        dataStream<<i;
        writeBasicSphereData(dataStream, spheres[i]);
    }
    emit frameToSend(frameData);
}

void SphereCalculator::updateSphereBox()
{
    if (spheres.size()>0)
    {
        Vector3 pos = spheres[0].pos;
        Scalar radius;
        Vector3 maxPos = pos, minPos = pos;
        for (quint16 i = 1; i<spheres.size(); i++)
        {
            pos = spheres[i].pos;
            radius = spheres[i].radius;
            if (pos(0)+radius>maxPos(0))
            {
                maxPos(0) = pos(0)+radius;
            }
            if (pos(1)+radius>maxPos(1))
            {
                maxPos(1) = pos(1)+radius;
            }
            if (pos(2)+radius>maxPos(2))
            {
                maxPos(2) = pos(2)+radius;
            }
            if (pos(0)-radius<minPos(0))
            {
                minPos(0) = pos(0)-radius;
            }
            if (pos(1)-radius<minPos(1))
            {
                minPos(1) = pos(1)-radius;
            }
            if (pos(2)-radius<minPos(2))
            {
                minPos(2) = pos(2)-radius;
            }
        }
        sphereBoxSize = maxPos-minPos;
        sphereBoxPosition = minPos;
    }
    else
    {
        sphereBoxSize = Vector3(0, 0, 0);
        sphereBoxPosition = Vector3(0, 0, 0);
    }
}

void SphereCalculator::updateSphereCellLists()
{
    for (quint32 i = 0; i<cellCount3; i++)
    {
        sphereIndicesInCells.resetCounter(i);
    }
    quint16 indexMinX, indexMinY, indexMinZ;
    quint16 indexMaxX, indexMaxY, indexMaxZ;
    quint32 indexAll;
    Scalar value, pos;
    Sphere s;
    for (quint16 i = 0; i<spheres.size(); i++)
    {
        s = spheres[i];
        cellIndicesOfSpheres.resetCounter(i);

        pos = s.pos(0);
        pos = fmin(pos, pos + timeStep*s.speed(0));
        pos = fmin(pos, pos + 0.5*s.acc(0)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(0)-s.radius)/sphereBoxSize(0);
        indexMinX = (quint16)(value*cellCount);
        indexMinX = (indexMinX<cellCount?indexMinX:cellCount-1);

        pos = s.pos(1);
        pos = fmin(pos, pos + timeStep*s.speed(1));
        pos = fmin(pos, pos + 0.5*s.acc(1)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(1)-s.radius)/sphereBoxSize(1);
        indexMinY = (quint16)(value*cellCount);
        indexMinY = (indexMinY<cellCount?indexMinY:cellCount-1);

        pos = s.pos(2);
        pos = fmin(pos, pos + timeStep*s.speed(2));
        pos = fmin(pos, pos + 0.5*s.acc(2)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(2)-s.radius)/sphereBoxSize(2);
        indexMinZ = (quint16)(value*cellCount);
        indexMinZ = (indexMinZ<cellCount?indexMinZ:cellCount-1);

        pos = s.pos(0);
        pos = fmax(pos, pos + timeStep*s.speed(0));
        pos = fmax(pos, pos + 0.5*s.acc(0)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(0)+s.radius)/sphereBoxSize(0);
        indexMaxX = (quint16)(value*cellCount);
        indexMaxX = (indexMaxX<cellCount?indexMaxX:cellCount-1);
        indexMaxX = (indexMaxX<indexMinX?indexMinX:indexMaxX);

        pos = s.pos(1);
        pos = fmax(pos, pos + timeStep*s.speed(1));
        pos = fmax(pos, pos + 0.5*s.acc(1)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(1)+s.radius)/sphereBoxSize(1);
        indexMaxY = (quint16)(value*cellCount);
        indexMaxY = (indexMaxY<cellCount?indexMaxY:cellCount-1);
        indexMaxY = (indexMaxY<indexMinY?indexMinY:indexMaxY);

        pos = s.pos(2);
        pos = fmax(pos, pos + timeStep*s.speed(2));
        pos = fmax(pos, pos + 0.5*s.acc(2)*timeStep*timeStep);
        value = (pos-sphereBoxPosition(2)+s.radius)/sphereBoxSize(2);
        indexMaxZ = (quint16)(value*cellCount);
        indexMaxZ = (indexMaxZ<cellCount?indexMaxZ:cellCount-1);
        indexMaxZ = (indexMaxZ<indexMinZ?indexMinZ:indexMaxZ);

        for (quint16 z = indexMinZ; z<=indexMaxZ; z++)
        {
            for (quint16 y = indexMinY; y<=indexMaxY; y++)
            {
                for (quint16 x = indexMinX; x<=indexMaxX; x++)
                {
                    indexAll = z*cellCount*cellCount + y*cellCount + x;
                    sphereIndicesInCells.addElement(indexAll, i);
                    cellIndicesOfSpheres.addElement(i, indexAll);
                }
            }
        }
    }
}

void SphereCalculator::updateSphereGravityCellLists()
{
    for (quint32 i = 0; i<gravityCellCount3; i++)
    {
        sphereIndicesInGravityCells.resetCounter(i);
    }
    quint16 indexX, indexY, indexZ;
    quint32 indexAll;
    Vector3 pos;
    Scalar value;
    for (quint16 i = 0; i<spheres.size(); i++)
    {
        pos = spheres[i].pos;
        value = (pos(0)-sphereBoxPosition(0))/sphereBoxSize(0);
        indexX = (quint16)(value*gravityCellCount);
        indexX = (indexX<gravityCellCount?indexX:gravityCellCount-1);
        value = (pos(1)-sphereBoxPosition(1))/sphereBoxSize(1);
        indexY = (quint16)(value*gravityCellCount);
        indexY = (indexY<gravityCellCount?indexY:gravityCellCount-1);
        value = (pos(2)-sphereBoxPosition(2))/sphereBoxSize(2);
        indexZ = (quint16)(value*gravityCellCount);
        indexZ = (indexZ<gravityCellCount?indexZ:gravityCellCount-1);
        indexAll = indexZ*gravityCellCount*gravityCellCount
            + indexY*gravityCellCount + indexX;
        sphereIndicesInGravityCells.addElement(indexAll, i);
        gravityCellIndexOfSpheres[i] = indexAll;
    }
}

void SphereCalculator::buildGravityCells()
{
    quint32 cellIndex;
    Scalar cellLength = 1.0/gravityCellCount;
    for (unsigned int x = 0; x < gravityCellCount; x++)
    {
        for (unsigned int y = 0; y < gravityCellCount; y++)
        {
            for (unsigned int z = 0; z < gravityCellCount; z++)
            {
                cellIndex = gravityCellCount3 + z*gravityCellCount*gravityCellCount
                    + y*gravityCellCount + x;
                gravityCellSizes[cellIndex] =
                    Vector3(cellLength, cellLength, cellLength);
                gravityCellPositions[cellIndex] =
                    Vector3(x*cellLength, y*cellLength, z*cellLength);
            }
        }
    }
    quint8 divisionDimension;
    for (cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
    {
        if (cellIndex < gravityCellCount3)
        {
            divisionDimension = 0;
            for (quint32 j = cellIndex; j>1; j/=2)
            {
                divisionDimension++;
            }
            divisionDimension %= 3;
            gravityCellSizes[cellIndex] = gravityCellSizes[2*cellIndex];
            gravityCellSizes[cellIndex](divisionDimension) *= 2;
            gravityCellPositions[cellIndex] = gravityCellPositions[2*cellIndex];
        }
        gravityCellHalfDiagonalLength[cellIndex] =
            gravityCellSizes[cellIndex].norm()/2;
    }
    for (cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
    {
        gravityCellPositions[cellIndex] += gravityCellSizes[cellIndex]/2;
    }
}

void SphereCalculator::rebuildGravityCellPairs()
{
    for (quint32 cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
    {
        if (cellIndex >= gravityCellCount3)
        {
            pairwiseCellsPerGravityCell.resetCounter(cellIndex);
            approximatingCellsPerGravityCell.resetCounter(cellIndex);
            approximatingCellsOffsetPerGravityCell.resetCounter(cellIndex);
            rebuildGravityCellPairs(cellIndex, 1);
        }
    }
}

void SphereCalculator::rebuildGravityCellPairs(quint32 currentCellIndex,
    quint32 testCellIndex)
{
    if (periodicBoundaryConditions)
    {
        if (testCellIndex >= gravityCellCount3)
        {
            pairwiseCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
        }
        else
        {
            rebuildGravityCellPairs(currentCellIndex, testCellIndex*2);
            rebuildGravityCellPairs(currentCellIndex, (testCellIndex*2)+1);
        }
        return;
    }
    if (currentCellIndex == testCellIndex)
    {
        //test cell is same
        pairwiseCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
        return;
    }
    else
    {
        quint32 index = currentCellIndex;
        while (index > testCellIndex)
        {
            index /= 2;
        }
        if (index == testCellIndex)
        {
            //test cell is parent
            rebuildGravityCellPairs(currentCellIndex, testCellIndex*2);
            rebuildGravityCellPairs(currentCellIndex, (testCellIndex*2)+1);
            return;
        }
    }
    Vector3 currentCellPos = gravityCellPositions[currentCellIndex];
    Vector3 testCellPos = gravityCellPositions[testCellIndex];
    Vector3 testCellOffset = Vector3();
    /*if (periodicBoundaryConditions)
    {
        Scalar cellDistance = (testCellPos-currentCellPos).norm(), newCellDistance;
        Vector3 modifiedTestCellPos;
        for (quint8 i = 0; i<6; i++)
        {
            quint8 dim = i/2;
            modifiedTestCellPos = testCellPos + testCellOffset;
            modifiedTestCellPos(dim) += (dim%2==0?1:-1)*simulatedSystem
                ->get<Vector3>(SimulationVariables::boxSize)(dim);
            newCellDistance = (modifiedTestCellPos-currentCellPos).norm();
            if (newCellDistance < cellDistance)
            {
                testCellOffset(dim) += (dim%2==0?1:-1)*simulatedSystem
                    ->get<Vector3>(SimulationVariables::boxSize)(dim);
                cellDistance = newCellDistance;
            }
        }
    }*/
    Scalar maxCellLength = 2*fmax(gravityCellHalfDiagonalLength[currentCellIndex],
        gravityCellHalfDiagonalLength[testCellIndex]);
    Scalar minimalDistance =
        fmax((currentCellPos-(testCellPos+testCellOffset)).norm()
        -gravityCellHalfDiagonalLength[currentCellIndex]
        -gravityCellHalfDiagonalLength[testCellIndex], 0.0000001);
    Scalar theta = maxCellLength / minimalDistance;
    if (theta<maximumTheta)
    {
        approximatingCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
        approximatingCellsOffsetPerGravityCell.addElement(currentCellIndex,
            testCellOffset);
    }
    else
    {
        if (testCellIndex >= gravityCellCount3)
        {
            pairwiseCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
        }
        else
        {
            rebuildGravityCellPairs(currentCellIndex, testCellIndex*2);
            rebuildGravityCellPairs(currentCellIndex, (testCellIndex*2)+1);
        }
    }
}

void SphereCalculator::updateGravityCellIndexOfSpheresArray()
{
    if (gravityCellIndexOfSpheres != nullptr)
    {
        delete[] gravityCellIndexOfSpheres;
    }
    gravityCellIndexOfSpheres = nullptr;
    quint16 count = spheres.size();
    if (count > 0)
    {
        gravityCellIndexOfSpheres = new quint32[count];
    }
}

void SphereCalculator::updateGravityCellData()
{
    quint32 cellIndex;
    for (cellIndex = 0; cellIndex<gravityAllCellCount; cellIndex++)
    {
        massVectorSumPerCell[cellIndex].setZero();
        massSumPerCell[cellIndex] = 0;
        sphereCountPerGravityCell[cellIndex] = 0;
    }
    quint32 sphereIndex;
    Sphere s;
    for (sphereIndex = 0; sphereIndex<spheres.size(); sphereIndex++)
    {
        s = spheres[sphereIndex];
        cellIndex = gravityCellCount3 + gravityCellIndexOfSpheres[sphereIndex];
        massVectorSumPerCell[cellIndex] += s.mass*s.pos;
        massSumPerCell[cellIndex] += s.mass;
        sphereCountPerGravityCell[cellIndex]++;
    }
    quint32 parentCellIndex;
    for (cellIndex = gravityAllCellCount-1; cellIndex > 1; cellIndex--)
    {
        if (massSumPerCell[cellIndex] != 0)
        {
            parentCellIndex = cellIndex/2;
            massVectorSumPerCell[parentCellIndex] += massVectorSumPerCell[cellIndex];
            massSumPerCell[parentCellIndex] += massSumPerCell[cellIndex];

            massCenterPerCell[cellIndex] =
                massVectorSumPerCell[cellIndex]/massSumPerCell[cellIndex];
            sphereCountPerGravityCell[parentCellIndex] +=
                sphereCountPerGravityCell[cellIndex];
        }
    }
}

quint16 SphereCalculator::addSphere()
{
    spheres.push_back(Sphere());
    newSpherePos.push_back(Vector3());
    cellIndicesOfSpheres.resize(spheres.size());
    collidingSpheresPerSphere.resize(spheres.size());
    updateGravityCellIndexOfSpheresArray();
    //workQueue->sendFrameData();
    return getAndUpdateSphereCount();
}

quint16 SphereCalculator::removeLastSphere()
{
    if (spheres.size()>0)
    {
        return removeSphere(spheres.size()-1);
    }
    else
    {
        return getAndUpdateSphereCount();
    }
}

quint16 SphereCalculator::updateSphere(quint16 i, Sphere s)
{
    if (spheres.size()>i)
    {
        spheres[i] = s;
        workQueue->sendFrameData();
    }
    return spheres.size();
}

Sphere SphereCalculator::getAllSphereData(quint16 i)
{
    if (spheres.size()>i)
    {
        return spheres[i];
    }
    else
    {
        return Sphere();
    }
}

quint16 SphereCalculator::addSomeSpheres(quint16 count)
{
    /*spheres.insert(spheres.size()-1, spheres.size(), Sphere());
    newSpherePos.insert(newSpherePos.size()-1, spheres.size(), Vector3());
    cellIndicesOfSpheres.resize(spheres.size());
    collidingSpheresPerSphere.resize(spheres.size());
    updateGravityCellIndexOfSpheresArray();
    workQueue->sendFrameData();
    return spheres.size();*/
    for (quint16 i = 0; i<count; i++)
    {
        addSphere();
    }
    return spheres.size();
}

quint16 SphereCalculator::removeSomeLastSpheres(quint16 count)
{
    for (quint16 i = 0; i<count; i++)
    {
        removeLastSphere();
    }
    return spheres.size();
}

void SphereCalculator::updateSpherePositionsInBox(Scalar randomDisplacement,
    Scalar randomSpeed)
{
    quint16 sphereCount1D = (quint16)ceil(pow(spheres.size(), 1/3.0));

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration timepoint = now.time_since_epoch();
    std::default_random_engine generator(timepoint.count());
    std::uniform_real_distribution<Scalar> distribution(-1, 1);

    for (unsigned int i = 0; i<spheres.size(); i++)
    {
        Console::out<<"SphereCalculator: sphere "<<(i+1)<<"|"<<spheres.size()<<"\r";
        Sphere& s = spheres[i];
        s.pos = boxSize/2;
        s.pos(0) += boxSize(0)/sphereCount1D
            *((sphereCount1D-1)/2.0-(i%sphereCount1D));
        s.pos(1) += boxSize(1)/sphereCount1D
            *((sphereCount1D-1)/2.0-((i/sphereCount1D)%sphereCount1D));
        s.pos(2) += boxSize(2)/sphereCount1D
            *((sphereCount1D-1)/2.0-((i/sphereCount1D)/sphereCount1D));
        s.speed.setZero();
        for (quint8 dim = 0; dim<3; dim++)
        {
            s.pos(dim) += s.radius*randomDisplacement*distribution(generator);
            s.speed(dim) += s.radius*randomSpeed*distribution(generator);
        }
    }
    qDebug()<<"SphereCalculator: updateSpherePositionsInBox finished.";
}

quint16 SphereCalculator::updateAllSpheres(Sphere s)
{
    for (quint16 i = 0; i<spheres.size(); i++)
    {
        spheres[i] = s;
    }
    workQueue->sendFrameData();
    return spheres.size();
}

void SphereCalculator::calculateStep()
{
    calculateSomeSteps(1);
}

void SphereCalculator::updateIntegratorMethod()
{
    quint8 integrMethod = integratorMethod;

    if (integrMethod == IntegratorMethods::HeunEuler21)
    {
        qDebug()<<"SphereCalculator: activated HeunEuler21 integrator.";
        const Scalar a[4] =
            {
                0.0,    0.0,
                1.0,    0.0
            };
        const Scalar b[2] =     {1/2.0, 1/2.0};
        const Scalar b_[2] =    {1.0,   0.0};
        const Scalar c[2] =     {0.0,   1.0};
        butcherTableau = ButcherTableau(2, a, b, b_, c);
    }
    else if (integrMethod == IntegratorMethods::BogackiShampine32)
    {
        qDebug()<<"SphereCalculator: activated BogackiShampine32 integrator.";
        const Scalar a[16] =
            {
                0.0,    0.0,    0.0,    0.0,
                1/2.0,  0.0,    0.0,    0.0,
                0.0,    3/4.0,  0.0,    0.0,
                2/9.0,  1/3.0,  4/9.0,  0.0
            };
        const Scalar b[4] =     {2/9.0,     1/3.0,  4/9.0,  0.0};
        const Scalar b_[4] =    {7/24.0,    1/4.0,  1/3.0,  1/8.0};
        const Scalar c[4] =     {0.0,       1/2.0,  3/4.0,  1.0};
        butcherTableau = ButcherTableau(4, a, b, b_, c);
    }
    else if (integrMethod == IntegratorMethods::CashKarp54)
    {
        qDebug()<<"SphereCalculator: activated CashKarp54 integrator.";
        const Scalar a[36] =
            {
                0.0,            0.0,        0.0,
                0.0,            0.0,        0.0,
                1/5.0,          0.0,        0.0,
                0.0,            0.0,        0.0,
                3/40.0,         9/40.0,     0.0,
                0.0,            0.0,        0.0,
                3/10.0,         -9/10.0,    6/5.0,
                0.0,            0.0,        0.0,
                -11/54.0,       5/2.0,      -70/27.0,
                35/27.0,        0.0,        0.0,
                1631/55296.0,   175/512.0,  575/13824.0,
                44275/110592.0, 253/4096.0, 0.0
            };
        const Scalar b[6] =
            {
                37/378.0,       0.0,        250/621.0,
                125/594.0,      0.0,        512/1771.0
            };
        const Scalar b_[6] =
            {
                2825/27648.0,   0.0,            18575/48384.0,
                13525/55296.0,  277/14336.0,    1/4.0
            };
        const Scalar c[6] =
            {
                0.0,            1/5.0,          3/10.0,
                3/5.0,          1.0,            7/8.0
            };
        butcherTableau = ButcherTableau(6, a, b, b_, c);
    }
    else if (integrMethod == IntegratorMethods::DormandPrince54)
    {
        qDebug()<<"SphereCalculator: activated DormandPrince54 integrator.";
        const Scalar a[49] =
            {
                0.0,            0.0,                0.0,
                0.0,            0.0,                0.0,            0.0,
                1/5.0,          0.0,                0.0,
                0.0,            0.0,                0.0,            0.0,
                3/40.0,         9/40.0,             0.0,
                0.0,            0.0,                0.0,            0.0,
                44/45.0,        -56/15.0,           32/9.0,
                0.0,            0.0,                0.0,            0.0,
                19372/6561.0,   -25360/2187.0,      64448/6561.0,
                -212/729.0,     0.0,                0.0,            0.0,
                9017/3168.0,    -355/33.0,          46732/5247.0,
                49/176.0,       -5103/18656.0,      0.0,            0.0,
                35/384.0,       0.0,                500/1113.0,
                125/192.0,      -2187/6784.0,       11/84.0,        0.0
            };
        const Scalar b[7] =
            {
                35/384.0,       0.0,                500/1113.0,
                125/192.0,      -2187/6784.0,       11/84.0,        0.0
            };
        const Scalar b_[7] =
            {
                5179/57600.0,   0.0,                7571/16695.0,
                393/640.0,      -92097/339200.0,    187/2100.0,     1/40.0
            };
        const Scalar c[7] =
            {
                0.0,            1/5.0,              3/10.0,
                4/5.0,          8/9.0,              1.0,            1.0
            };
        butcherTableau = ButcherTableau(7, a, b, b_, c);
    }
    else
    {
        qDebug()<<"SphereCalculator: activated RungeKuttaFehlberg54 integrator.";
        simulatedSystem->set(SimulationVariables::integratorMethod,
            (int)IntegratorMethods::RungeKuttaFehlberg54);
        const Scalar a[36] =
            {
                0.0,            0.0,            0.0,
                0.0,            0.0,            0.0,
                1/4.0,          0.0,            0.0,
                0.0,            0.0,            0.0,
                3/32.0,         9/32.0,         0.0,
                0.0,            0.0,            0.0,
                1932/2197.0,    -7200/2197.0,   7296/2197.0,
                0.0,            0.0,            0.0,
                439/216.0,      -8.0,           3680/513.0,
                -845/4104.0,    0.0,            0.0,
                -8/27.0,        2.0,            -3544/2565.0,
                1859/4104.0,    -11/40.0,       0.0
            };
        const Scalar b[6] =
            {
                16/135.0,       0.0,            6656/12825.0,
                28561/56430.0,  -9/50.0,        2/55.0
            };
        const Scalar b_[6] =
            {
                25/216.0,       0.0,            1408/2565.0,
                2197/4104.0,    -1/5.0,         0.0
            };
        const Scalar c[6] =
            {
                0.0,            1/4.0,          3/8.0,
                12/13.0,        1.0,            1/2.0
            };
        butcherTableau = ButcherTableau(6, a, b, b_, c);
    }
}

quint32 SphereCalculator::popCalculationCounter()
{
    if (spheres.size()>0)
    {
        quint32 counter = calculationCounter/spheres.size();
        calculationCounter = 0;
        return counter;
    }
    else
    {
        return 0;
    }
}

void SphereCalculator::calculateSomeSteps(quint32 steps)
{
    workQueue->pushSimulationSteps(steps);
}

void SphereCalculator::startSimulation()
{
    calculateSomeSteps(0);
}

void SphereCalculator::stopSimulation()
{
    emit requestingSimulationStop();
}

quint32 SphereCalculator::popStepCounter()
{
    quint32 counter = stepCounter;
    stepCounter = 0;
    return counter;
}

quint32 SphereCalculator::getLastStepCalculationTime()
{
    return lastStepCalculationTime;
}

Scalar SphereCalculator::getTotalEnergy()
{
    if (collisionDetection)
    {
        if (gravityCalculation)
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<true, true, true, true>();
                }
                else
                {
                    return getTotalEnergy_internal<true, true, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<true, true, false, true>();
                }
                else
                {
                    return getTotalEnergy_internal<true, true, false, false>();
                }
            }
        }
        else
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<true, false, true, true>();
                }
                else
                {
                    return getTotalEnergy_internal<true, false, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<true, false, false, true>();
                }
                else
                {
                    return getTotalEnergy_internal<true, false, false, false>();
                }
            }
        }
    }
    else
    {
        if (gravityCalculation)
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<false, true, true, true>();
                }
                else
                {
                    return getTotalEnergy_internal<false, true, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<false, true, false, true>();
                }
                else
                {
                    return getTotalEnergy_internal<false, true, false, false>();
                }
            }
        }
        else
        {
            if (lennardJonesPotential)
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<false, false, true, true>();
                }
                else
                {
                    return getTotalEnergy_internal<false, false, true, false>();
                }
            }
            else
            {
                if (periodicBoundaryConditions)
                {
                    return getTotalEnergy_internal<false, false, false, true>();
                }
                else
                {
                    return getTotalEnergy_internal<false, false, false, false>();
                }
            }
        }
    }
}

Scalar SphereCalculator::getKineticEnergy()
{
    Scalar totalEnergy = 0.0, sphereEnergy;
    Sphere sphere;
    for (quint16 sphereIndex = 0; sphereIndex<spheres.size(); ++sphereIndex)
    {
        sphere = spheres[sphereIndex];
        sphereEnergy = 0.5*sphere.mass*sphere.speed.squaredNorm();
        totalEnergy += sphereEnergy;
    }
    return totalEnergy;
}

void SphereCalculator::updateKineticEnergy(Scalar factor)
{
    factor = sqrt(factor);
    for (quint16 sphereIndex = 0; sphereIndex<spheres.size(); sphereIndex++)
    {
        spheres[sphereIndex].speed *= factor;
    }
}

void SphereCalculator::updateTargetTemperature()
{
    Scalar factor = 3*spheres.size()*kBoltzmann*targetTemperature
        /(2*getKineticEnergy());
    updateKineticEnergy(factor);
}

void SphereCalculator::updateSphereSphereE()
{
    sphereSphereE = 1/(((1-spherePoissonRatio*spherePoissonRatio)/sphereE)
        +((1-spherePoissonRatio*spherePoissonRatio)/sphereE));
}

void SphereCalculator::updateSphereWallE()
{
    sphereWallE = 1/(((1-spherePoissonRatio*spherePoissonRatio)/sphereE)
        +((1-wallPoissonRatio*wallPoissonRatio)/wallE));
}

void SphereCalculator::variableUpdated(int var)
{
    switch (var)
    {
    case SimulationVariables::sphereE:
    case SimulationVariables::spherePoissonRatio:
        updateSphereSphereE();
    case SimulationVariables::wallE:
    case SimulationVariables::wallPoissonRatio:
        updateSphereWallE();
        break;
    case SimulationVariables::integratorMethod:
        updateIntegratorMethod();
        break;
    }
}
