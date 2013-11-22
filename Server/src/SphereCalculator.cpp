/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <SphereCalculator.hpp>
#include <SimulationWorker.hpp>
#include <WorkQueue.hpp>
#include <Console.hpp>
#include <SphereTransmit.hpp>

#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <cmath>
#include <omp.h>
#include <random>
#include <chrono>

using namespace SphereSim;

SphereCalculator::SphereCalculator():cellCount(8), cellCount3((quint32)cellCount*cellCount*cellCount),
	maxSpheresPerCell(1024), maxCellsPerSphere(1024),
	sphereIndicesInCells(maxSpheresPerCell, cellCount3), cellIndicesOfSpheres(maxCellsPerSphere),
	maxCollidingSpheresPerSphere(300), collidingSpheresPerSphere(maxCollidingSpheresPerSphere),
	gravityCellCount(4), gravityCellCount3(gravityCellCount*gravityCellCount*gravityCellCount),
	gravityAllCellCount(2*gravityCellCount3), maxSpheresPerGravityCell(1024),
	sphereIndicesInGravityCells(maxSpheresPerGravityCell, gravityCellCount3), maxApproximatingCellsPerGravityCell(gravityCellCount3),
	approximatingCellsPerGravityCell(maxApproximatingCellsPerGravityCell, gravityAllCellCount),
	approximatingCellsOffsetPerGravityCell(maxApproximatingCellsPerGravityCell, gravityAllCellCount),
	maxPairwiseCellsPerGravityCell(gravityCellCount3), pairwiseCellsPerGravityCell(maxPairwiseCellsPerGravityCell, gravityAllCellCount)
{
	qDebug()<<"SphereCalculator: constructor called";
	quint16 ompThreads = 0;
	#pragma omp parallel
	#pragma omp atomic
	ompThreads++;
	qDebug()<<"SphereCalculator: number of OpenMP threads:"<<omp_get_num_threads()<<"|"<<ompThreads;
	updateData();
	simulatedSystem.boxSize = Vector3(1,1,1);
	timeStep = 0.002;
	updateIntegratorMethod(IntegratorMethods::CashKarp54);
	calculationCounter = 0;
	stepCounter = 0;
	updateSphereE(5000);
	updateSpherePoissonRatio(0.5);
	updateWallE(5000);
	updateWallPoissonRatio(0.5);
	updateEarthGravity(Vector3(0, -9.81, 0));
	collisionDetectionFlag = true;
	gravityCalculationFlag = false;
	simulatedSystem.gravitationalConstant = 6.67384e-11;
	simulatedSystem.maximumTheta = 0.5;
	lennardJonesPotentialFlag = false;
	simulatedSystem.lenJonPotEpsilon = 1.6540e-21;
	simulatedSystem.lenJonPotSigma = 0.3405e-9;
	simulatedSystem.periodicBoundaryConditions = false;
	simulatedSystem.kBoltzmann = 1.3806504e-23;
	maxStepDivision = 16;
	maxStepError = 1.0e-4;
	lastStepCalculationTime = 0;
	elapsedTimer = new QElapsedTimer();
	
	updateSphereBox();
	massVectorSumPerCell = new Vector3[gravityAllCellCount];
	massSumPerCell = new Scalar[gravityAllCellCount];
	massCenterPerCell = new Vector3[gravityAllCellCount];
	gravityCellSizes = new Vector3[gravityAllCellCount];
	gravityCellHalfDiagonalLength = new Scalar[gravityAllCellCount];
	gravityCellPositions = new Vector3[gravityAllCellCount];
	gravityCellIndexOfSpheres = NULL;
	sphereCountPerGravityCell = new quint16[gravityAllCellCount];
	updateGravityCellIndexOfSpheresArray();
	buildGravityCells();
	rebuildGravityCellPairs();
	
	workQueueMutex = new QMutex();
	workQueue = new WorkQueue(workQueueMutex);
	simulationWorker = new SimulationWorker(this, workQueue);
	simulationThread = new QThread();
	simulationWorker->moveToThread(simulationThread);
	QObject::connect(simulationThread, SIGNAL(started()), simulationWorker, SLOT(work()));
	QObject::connect(simulationWorker, SIGNAL(finished()), simulationThread, SLOT(quit()));
	QObject::connect(simulationWorker, SIGNAL(finished()), simulationWorker, SLOT(deleteLater()));
	QObject::connect(simulationThread, SIGNAL(finished()), simulationThread, SLOT(deleteLater()));
	QObject::connect(this, SIGNAL(requestingSimulationStop()), workQueue, SLOT(stopSimulation()));
	QObject::connect(this, SIGNAL(requestingWorkerStop()), simulationWorker, SLOT(stop()));
	QObject::connect(this, SIGNAL(requestingWorkerStop()), workQueue, SLOT(stop()));
	simulationThread->start();
}

SphereCalculator::~SphereCalculator()
{
	stopWorker();
	while(!simulationWorker->getHasFinished());
	delete[] massVectorSumPerCell;
	delete[] massSumPerCell;
	delete[] massCenterPerCell;
	delete[] gravityCellSizes;
	delete[] gravityCellHalfDiagonalLength;
	delete[] gravityCellPositions;
	if(gravityCellIndexOfSpheres != NULL)
		delete[] gravityCellIndexOfSpheres;
	delete[] sphereCountPerGravityCell;
	delete elapsedTimer;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential, bool periodicBoundaries>
Vector3 SphereCalculator::sphereAcceleration(quint16 sphereIndex, Sphere sphere, Scalar timeDiff)
{
	Scalar d, forceNormalized;
	Vector3 force, acc, dVec, dNormalized;
	quint16 sphereIndex2;
	Sphere sphere2;

	force = simulatedSystem.earthGravity*sphere.mass;
	for(quint8 dim = 0; dim<3; dim++)
	{
		if((d = (sphere.radius - sphere.pos(dim))) > 0)
		{
			forceNormalized = 4.0/3.0*simulatedSystem.E_sphere_wall*sqrt(sphere.radius*d*d*d);
			force(dim) += forceNormalized;
		}
		if((d = (sphere.radius + sphere.pos(dim) - simulatedSystem.boxSize(dim))) > 0)
		{
			forceNormalized = 4.0/3.0*simulatedSystem.E_sphere_wall*sqrt(sphere.radius*d*d*d);
			force(dim) -= forceNormalized;
		}
	}
	if(detectCollisions)
	{
		quint32 cellIndex;
		Scalar bothRadii, dOverlapping, R;
		collidingSpheresPerSphere.resetCounter(sphereIndex);
		for(int i = cellIndicesOfSpheres.getCounter(sphereIndex)-1; i>=0; i--)
		{
			cellIndex = cellIndicesOfSpheres[sphereIndex][i];
			for(int j = sphereIndicesInCells.getCounter(cellIndex)-1; j>=0; j--)
			{
				sphereIndex2 = sphereIndicesInCells[cellIndex][j];
				if(sphereIndex2 != sphereIndex)
				{
					if(collidingSpheresPerSphere.addElementIfNotContained(sphereIndex, sphereIndex2))
					{
						sphere2 = sphArr[sphereIndex2];
						sphere2.pos += timeDiff*sphere2.speed;// + 0.5*sphere2.acc*timeDiff*timeDiff;
						dVec = sphere2.pos - sphere.pos;
						d = dVec.norm();
						bothRadii = sphere2.radius + sphere.radius;
						if(d < bothRadii)
						{
							dNormalized = dVec/d;
							dOverlapping = bothRadii - d;
							R = 1/((1/sphere.radius)+(1/sphere2.radius));
							forceNormalized = 4.0f/3.0f*simulatedSystem.E_sphere_sphere*sqrt(R*dOverlapping*dOverlapping*dOverlapping);
							force -= forceNormalized*dNormalized;
						}
					}
				}
			}
		}
	}
	
	if(gravity || lennardJonesPotential)
	{
		quint32 gravityCellIndex = gravityCellCount3 + gravityCellIndexOfSpheres[sphereIndex];
		quint32 gravityCellIndex2;
		
		Vector3 sphereOffset;
		Vector3 sphereTestPos;
		Vector3 sphereTestPos2;
		Vector3 boxSize = simulatedSystem.boxSize;
		Vector3 dVecNew;
		Scalar dNew;
		for(int i = pairwiseCellsPerGravityCell.getCounter(gravityCellIndex)-1; i>=0; i--)
		{
			gravityCellIndex2 = pairwiseCellsPerGravityCell[gravityCellIndex][i] - gravityCellCount3;
			for(int j = sphereIndicesInGravityCells.getCounter(gravityCellIndex2)-1; j>=0; j--)
			{
				sphereIndex2 = sphereIndicesInGravityCells[gravityCellIndex2][j];
				if(sphereIndex == sphereIndex2)
					continue;
				sphere2 = sphArr[sphereIndex2];
				sphere2.pos += timeDiff*sphere2.speed;// + 0.5*sphere2.acc*timeDiff*timeDiff;
				if(periodicBoundaries)
				{
					sphereOffset.setZero();
					dVec = sphere2.pos-sphere.pos;
					d = dVec.norm();
					sphereTestPos = sphere2.pos;
					for(quint8 dim = 0; dim<3; dim++)
					{
						sphereTestPos2 = sphereTestPos;
						sphereTestPos2(dim) += boxSize(dim);
						dVecNew = sphereTestPos2-sphere.pos;
						dNew = dVecNew.norm();
						if(dNew < d)
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
							if(dNew < d)
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
				if(gravity)
					force += simulatedSystem.gravitationalConstant * sphere.mass * sphere2.mass * dVec / d / d / d;
				if(lennardJonesPotential)
				{
					force -= 48*simulatedSystem.lenJonPotEpsilon/(simulatedSystem.lenJonPotSigma*simulatedSystem.lenJonPotSigma)
						*dVec*(pow(simulatedSystem.lenJonPotSigma/d, 14)-0.5*pow(simulatedSystem.lenJonPotSigma/d, 8));
				}
			}
		}
		for(int i = approximatingCellsPerGravityCell.getCounter(gravityCellIndex)-1; i>=0; i--)
		{
			gravityCellIndex2 = approximatingCellsPerGravityCell[gravityCellIndex][i];
			if(periodicBoundaries)
				dVec = (massCenterPerCell[gravityCellIndex2]+approximatingCellsOffsetPerGravityCell[gravityCellIndex][i])-sphere.pos;
			else
				dVec = massCenterPerCell[gravityCellIndex2]-sphere.pos;
			d = dVec.norm();
			if(gravity)
				force += simulatedSystem.gravitationalConstant * sphere.mass * massSumPerCell[gravityCellIndex2] * dVec / d / d / d;
			if(lennardJonesPotential)
			{
				force -= 48*simulatedSystem.lenJonPotEpsilon/(simulatedSystem.lenJonPotSigma*simulatedSystem.lenJonPotSigma)
					*dVec*(pow(simulatedSystem.lenJonPotSigma/d, 14)-0.5*pow(simulatedSystem.lenJonPotSigma/d, 8))
					*sphereCountPerGravityCell[gravityCellIndex2];
			}
		}
	}
	
	acc = force/sphere.mass;
	#pragma omp atomic
	calculationCounter++;
	return acc;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential, bool periodicBoundaries>
Scalar SphereCalculator::getTotalEnergy_internal()
{
	if(detectCollisions || gravity || lennardJonesPotential)
		updateSphereBox();
	if(detectCollisions)
		updateSphereCellLists();
	if(gravity || lennardJonesPotential)
	{
		updateSphereGravityCellLists();
		updateGravityCellData();
	}
	
	Scalar totalEnergy = 0.0, sphereEnergy, d;
	Sphere sphere;
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex)
	{
		sphere = sphArr[sphereIndex];
		sphereEnergy = -sphere.mass*simulatedSystem.earthGravity.dot(sphere.pos);
		sphereEnergy += 0.5*sphere.mass*sphere.speed.squaredNorm();
		
		for(quint8 dim = 0; dim<3; dim++)
		{
			if((d = (sphere.radius - sphere.pos(dim))) > 0)
			{
				sphereEnergy += 8.0/15.0*simulatedSystem.E_sphere_wall*sqrt(sphere.radius)*pow(d, 2.5);
			}
			if((d = (sphere.radius + sphere.pos(dim) - simulatedSystem.boxSize(dim))) > 0)
			{
				sphereEnergy += 8.0/15.0*simulatedSystem.E_sphere_wall*sqrt(sphere.radius)*pow(d, 2.5);
			}
		}
		
		if(detectCollisions)
		{
			quint32 cellIndex;
			quint16 sphereIndex2;
			Sphere sphere2;
			Vector3 dVec;
			Scalar d, bothRadii, dOverlapping, R, energy;
			collidingSpheresPerSphere.resetCounter(sphereIndex);
			for(int i = cellIndicesOfSpheres.getCounter(sphereIndex)-1; i>=0; i--)
			{
				cellIndex = cellIndicesOfSpheres[sphereIndex][i];
				for(int j = sphereIndicesInCells.getCounter(cellIndex)-1; j>=0; j--)
				{
					sphereIndex2 = sphereIndicesInCells[cellIndex][j];
					if(sphereIndex2 != sphereIndex)
					{
						if(collidingSpheresPerSphere.addElementIfNotContained(sphereIndex, sphereIndex2))
						{
							sphere2 = sphArr[sphereIndex2];
							dVec = sphere2.pos - sphere.pos;
							d = dVec.norm();
							bothRadii = sphere2.radius + sphere.radius;
							if(d < bothRadii)
							{
								dOverlapping = bothRadii - d;
								R = 1/((1/sphere.radius)+(1/sphere2.radius));
								energy = 8.0/15.0*simulatedSystem.E_sphere_sphere*sqrt(R)*pow(dOverlapping, 2.5);
								sphereEnergy += energy;
							}
						}
					}
				}
			}
		}
	
		if(gravity || lennardJonesPotential)
		{
			quint32 gravityCellIndex = gravityCellCount3 + gravityCellIndexOfSpheres[sphereIndex];
			quint32 gravityCellIndex2;
			quint16 sphereIndex2;
			Sphere sphere2;
			Vector3 dVec;
			for(int i = pairwiseCellsPerGravityCell.getCounter(gravityCellIndex)-1; i>=0; i--)
			{
				gravityCellIndex2 = pairwiseCellsPerGravityCell[gravityCellIndex][i] - gravityCellCount3;
				for(int j = sphereIndicesInGravityCells.getCounter(gravityCellIndex2)-1; j>=0; j--)
				{
					sphereIndex2 = sphereIndicesInGravityCells[gravityCellIndex2][j];
					if(sphereIndex == sphereIndex2)
						continue;
					sphere2 = sphArr[sphereIndex2];
					dVec = sphere2.pos-sphere.pos;
					d = dVec.norm();
					if(gravity)
						sphereEnergy -= simulatedSystem.gravitationalConstant * sphere.mass * sphere2.mass / d;
					if(lennardJonesPotential)
					{
						sphereEnergy += 4*simulatedSystem.lenJonPotEpsilon
							*(pow(simulatedSystem.lenJonPotSigma/d, 12)-pow(simulatedSystem.lenJonPotSigma/d, 6));
					}
				}
			}
			for(int i = approximatingCellsPerGravityCell.getCounter(gravityCellIndex)-1; i>=0; i--)
			{
				gravityCellIndex2 = approximatingCellsPerGravityCell[gravityCellIndex][i];
				if(periodicBoundaries)
					dVec = (massCenterPerCell[gravityCellIndex2]+approximatingCellsOffsetPerGravityCell[gravityCellIndex][i])-sphere.pos;
				else
					dVec = massCenterPerCell[gravityCellIndex2]-sphere.pos;
				d = dVec.norm();
				if(gravity)
					sphereEnergy -= simulatedSystem.gravitationalConstant * sphere.mass * massSumPerCell[gravityCellIndex2] / d;
				if(lennardJonesPotential)
				{
					sphereEnergy += 4*simulatedSystem.lenJonPotEpsilon
						*(pow(simulatedSystem.lenJonPotSigma/d, 12)-pow(simulatedSystem.lenJonPotSigma/d, 6))
						*sphereCountPerGravityCell[gravityCellIndex2];
				}
			}
		}
		
		totalEnergy += sphereEnergy;
	}
	return totalEnergy;
}

void SphereCalculator::updateData()
{
	sphArr = spheres.data();
	newSpherePosArr = newSpherePos.data();
	sphCount = spheres.count();
}

void SphereCalculator::integrateRungeKuttaStep()
{
	elapsedTimer->start();
	if(collisionDetectionFlag)
	{
		if(gravityCalculationFlag)
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<true, true, true, true>();
				else
					integrateRungeKuttaStep_internal<true, true, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<true, true, false, true>();
				else
					integrateRungeKuttaStep_internal<true, true, false, false>();
			}
		}
		else
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<true, false, true, true>();
				else
					integrateRungeKuttaStep_internal<true, false, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<true, false, false, true>();
				else
					integrateRungeKuttaStep_internal<true, false, false, false>();
			}
		}
	}
	else
	{
		if(gravityCalculationFlag)
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<false, true, true, true>();
				else
					integrateRungeKuttaStep_internal<false, true, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<false, true, false, true>();
				else
					integrateRungeKuttaStep_internal<false, true, false, false>();
			}
		}
		else
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<false, false, true, true>();
				else
					integrateRungeKuttaStep_internal<false, false, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					integrateRungeKuttaStep_internal<false, false, false, true>();
				else
					integrateRungeKuttaStep_internal<false, false, false, false>();
			}
		}
	}
	lastStepCalculationTime = elapsedTimer->elapsed()+1;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential, bool periodicBoundaries>
void SphereCalculator::integrateRungeKuttaStep_internal()
{
	if(detectCollisions || gravity || lennardJonesPotential)
		updateSphereBox();
	if(detectCollisions)
		updateSphereCellLists();
	if(gravity || lennardJonesPotential)
	{
		updateSphereGravityCellLists();
		updateGravityCellData();
	}
	#pragma omp parallel for schedule(dynamic,1)
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex)
	{
		newSpherePosArr[sphereIndex] = sphArr[sphereIndex].pos;
		integrateRungeKuttaStep_internal<detectCollisions, gravity, lennardJonesPotential, periodicBoundaries>(sphereIndex, timeStep, 0.0, 0);
	}
	#pragma omp parallel for schedule(dynamic,1)
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex)
	{
		if(periodicBoundaries)
		{
			Vector3 pos = newSpherePosArr[sphereIndex];
			for(quint8 dim = 0; dim<3; dim++)
			{
				if(pos(dim) > simulatedSystem.boxSize(dim))
					pos(dim) -= simulatedSystem.boxSize(dim);
				else if(pos(dim) < 0)
					pos(dim) += simulatedSystem.boxSize(dim);
			}
			sphArr[sphereIndex].pos = pos;
		}
		else
		{
			sphArr[sphereIndex].pos = newSpherePosArr[sphereIndex];
		}
	}
	stepCounter++;
}

template <bool detectCollisions, bool gravity, bool lennardJonesPotential, bool periodicBoundaries>
quint32 SphereCalculator::integrateRungeKuttaStep_internal(quint16 sphereIndex, Scalar stepLength, Scalar timeDiff, quint16 stepDivisionCounter)
{
	Sphere sphere = sphArr[sphereIndex];
	sphere.pos = newSpherePosArr[sphereIndex];
	Sphere origSphere = sphere;
	const quint8 integratorOrder = butcherTableau.order;
	Vector3 k_acc[integratorOrder];
	Vector3 k_speed[integratorOrder];
	
	k_acc[0] = sphereAcceleration<detectCollisions, gravity, lennardJonesPotential, periodicBoundaries>(sphereIndex, sphere, timeDiff);
	k_speed[0] = sphere.speed;
	for(quint8 n = 1; n<integratorOrder; n++)
	{
		sphere.pos = origSphere.pos;
		for(quint8 j = 0; j<n; j++)
		{
			sphere.pos += stepLength*butcherTableau.a[n][j]*k_speed[j];
		}
		k_acc[n] = sphereAcceleration<detectCollisions, gravity, lennardJonesPotential, periodicBoundaries>(sphereIndex, sphere, timeDiff);
		
		k_speed[n] = origSphere.speed;
		for(quint8 j = 0; j<n; j++)
		{
			k_speed[n] += stepLength*butcherTableau.a[n][j]*k_acc[j];
		}
	}
	
	Vector3 pos = origSphere.pos;
	Vector3 pos_ = pos;
	Vector3 speed = origSphere.speed;
	Vector3 speed_ = speed;
	for(quint8 j = 0; j<integratorOrder; j++)
	{
		pos += stepLength*butcherTableau.b[j]*k_speed[j];
		pos_ += stepLength*butcherTableau.b_[j]*k_speed[j];
		speed += stepLength*butcherTableau.b[j]*k_acc[j];
		speed_ += stepLength*butcherTableau.b_[j]*k_acc[j];
	}
	
	if(stepDivisionCounter < maxStepDivision)
	{
		Scalar error_pos_ = (pos-pos_).norm();
		Scalar error_speed_ = (speed-speed_).norm();
		Scalar dPos = (pos-origSphere.pos).norm();
		Scalar dSpeed = (speed-origSphere.speed).norm();
		if(error_pos_ > dPos*maxStepError || error_speed_ > dSpeed*maxStepError)
		{
			quint32 stepCount = 0;
			stepCount += integrateRungeKuttaStep_internal<detectCollisions, gravity, lennardJonesPotential, periodicBoundaries>(sphereIndex, stepLength/2, timeDiff, stepDivisionCounter+1);
			stepCount += integrateRungeKuttaStep_internal<detectCollisions, gravity, lennardJonesPotential, periodicBoundaries>(sphereIndex, stepLength/2, timeDiff+(stepLength/2), stepDivisionCounter+1);
			return stepCount;
		}
	}
	newSpherePosArr[sphereIndex] = pos;
	sphArr[sphereIndex].speed = speed;
	sphArr[sphereIndex].acc = (speed-origSphere.speed)/stepLength;
	return 1;
}

quint16 SphereCalculator::removeSphere(quint16 i)
{
	if(getSphereCount()>i)
	{
		spheres.remove(i);
		newSpherePos.remove(i);
		cellIndicesOfSpheres.changeSize(spheres.count());
		collidingSpheresPerSphere.changeSize(spheres.count());
		updateGravityCellIndexOfSpheresArray();
		updateData();
	}
	return getSphereCount();
}

void SphereCalculator::stopWorker()
{
	emit requestingWorkerStop();
}

void SphereCalculator::updateSphereSphereE()
{
	simulatedSystem.E_sphere_sphere = 1/(((1-simulatedSystem.poisson_sphere*simulatedSystem.poisson_sphere)/simulatedSystem.E_sphere)
		+((1-simulatedSystem.poisson_sphere*simulatedSystem.poisson_sphere)/simulatedSystem.E_sphere));
}

void SphereCalculator::updateSphereWallE()
{
	simulatedSystem.E_sphere_wall = 1/(((1-simulatedSystem.poisson_sphere*simulatedSystem.poisson_sphere)/simulatedSystem.E_sphere)
		+((1-simulatedSystem.poisson_wall*simulatedSystem.poisson_wall)/simulatedSystem.E_wall));
}

void SphereCalculator::prepareFrameData()
{
	QByteArray frameData;
	QDataStream dataStream(&frameData, QIODevice::WriteOnly);
	//updateData();
	dataStream<<sphCount;
	for(quint16 i = 0; i<sphCount; i++)
	{
		dataStream<<i;
		writeBasicSphereData(dataStream, sphArr[i]);
	}
	emit frameToSend(frameData);
}

void SphereCalculator::updateSphereBox()
{
	if(spheres.count()>0)
	{
		Vector3 pos = sphArr[0].pos;
		Scalar radius;
		Vector3 max = pos, min = pos;
		for(quint16 i = 1; i<spheres.count(); i++)
		{
			pos = sphArr[i].pos;
			radius = sphArr[i].radius;
			if(pos(0)+radius>max(0))
				max(0) = pos(0)+radius;
			if(pos(1)+radius>max(1))
				max(1) = pos(1)+radius;
			if(pos(2)+radius>max(2))
				max(2) = pos(2)+radius;
			if(pos(0)-radius<min(0))
				min(0) = pos(0)-radius;
			if(pos(1)-radius<min(1))
				min(1) = pos(1)-radius;
			if(pos(2)-radius<min(2))
				min(2) = pos(2)-radius;
		}
		sphereBoxSize = max-min;
		sphereBoxPosition = min;
	}else{
		sphereBoxSize = Vector3(0, 0, 0);
		sphereBoxPosition = Vector3(0, 0, 0);
	}
}

void SphereCalculator::updateSphereCellLists()
{
	for(quint32 i = 0; i<cellCount3; i++)
	{
		sphereIndicesInCells.resetCounter(i);
	}
	quint16 indexMinX, indexMinY, indexMinZ;
	quint16 indexMaxX, indexMaxY, indexMaxZ;
	quint32 indexAll;
	Scalar value, pos;
	Sphere s;
	for(quint16 i = 0; i<sphCount; i++)
	{
		s = sphArr[i];
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
		
		for(quint16 z = indexMinZ; z<=indexMaxZ; z++)
		{
			for(quint16 y = indexMinY; y<=indexMaxY; y++)
			{
				for(quint16 x = indexMinX; x<=indexMaxX; x++)
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
	for(quint32 i = 0; i<gravityCellCount3; i++)
	{
		sphereIndicesInGravityCells.resetCounter(i);
	}
	quint16 indexX, indexY, indexZ;
	quint32 indexAll;
	Vector3 pos;
	Scalar value;
	for(quint16 i = 0; i<sphCount; i++)
	{
		pos = sphArr[i].pos;
		value = (pos(0)-sphereBoxPosition(0))/sphereBoxSize(0);
		indexX = (quint16)(value*gravityCellCount);
		indexX = (indexX<gravityCellCount?indexX:gravityCellCount-1);
		value = (pos(1)-sphereBoxPosition(1))/sphereBoxSize(1);
		indexY = (quint16)(value*gravityCellCount);
		indexY = (indexY<gravityCellCount?indexY:gravityCellCount-1);
		value = (pos(2)-sphereBoxPosition(2))/sphereBoxSize(2);
		indexZ = (quint16)(value*gravityCellCount);
		indexZ = (indexZ<gravityCellCount?indexZ:gravityCellCount-1);
		indexAll = indexZ*gravityCellCount*gravityCellCount + indexY*gravityCellCount + indexX;
		sphereIndicesInGravityCells.addElement(indexAll, i);
		gravityCellIndexOfSpheres[i] = indexAll;
	}
}

void SphereCalculator::buildGravityCells()
{
	quint32 cellIndex;
	Scalar cellLength = 1.0/gravityCellCount;
	for(unsigned int x = 0; x < gravityCellCount; x++)
	{
		for(unsigned int y = 0; y < gravityCellCount; y++)
		{
			for(unsigned int z = 0; z < gravityCellCount; z++)
			{
				cellIndex = gravityCellCount3 + z*gravityCellCount*gravityCellCount + y*gravityCellCount + x;
				gravityCellSizes[cellIndex] = Vector3(cellLength, cellLength, cellLength);
				gravityCellPositions[cellIndex] = Vector3(x*cellLength, y*cellLength, z*cellLength);
			}
		}
	}
	quint8 divisionDimension;
	for(cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
	{
		if(cellIndex < gravityCellCount3)
		{
			divisionDimension = 0;
			for(quint32 j = cellIndex; j>1; j/=2)
			{
				divisionDimension++;
			}
			divisionDimension %= 3;
			gravityCellSizes[cellIndex] = gravityCellSizes[2*cellIndex];
			gravityCellSizes[cellIndex](divisionDimension) *= 2;
			gravityCellPositions[cellIndex] = gravityCellPositions[2*cellIndex];
		}
		gravityCellHalfDiagonalLength[cellIndex] = gravityCellSizes[cellIndex].norm()/2;
	}
	for(cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
	{
		gravityCellPositions[cellIndex] += gravityCellSizes[cellIndex]/2;
	}
}

void SphereCalculator::rebuildGravityCellPairs()
{
	for(quint32 cellIndex = gravityAllCellCount-1; cellIndex >= 1; cellIndex--)
	{
		if(cellIndex >= gravityCellCount3)
		{
			pairwiseCellsPerGravityCell.resetCounter(cellIndex);
			approximatingCellsPerGravityCell.resetCounter(cellIndex);
			approximatingCellsOffsetPerGravityCell.resetCounter(cellIndex);
			rebuildGravityCellPairs(cellIndex, 1);
		}
	}
}

void SphereCalculator::rebuildGravityCellPairs(quint32 currentCellIndex, quint32 testCellIndex)
{
	if(simulatedSystem.periodicBoundaryConditions)
	{
		if(testCellIndex >= gravityCellCount3)
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
	if(currentCellIndex == testCellIndex)
	{
		//test cell is same
		pairwiseCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
		return;
	}
	else
	{
		quint32 index = currentCellIndex;
		while(index > testCellIndex)
			index /= 2;
		if(index == testCellIndex)
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
	/*if(simulatedSystem.periodicBoundaryConditions)
	{
		Scalar cellDistance = (testCellPos-currentCellPos).norm(), newCellDistance;
		Vector3 modifiedTestCellPos;
		for(quint8 i = 0; i<6; i++)
		{
			quint8 dim = i/2;
			modifiedTestCellPos = testCellPos + testCellOffset;
			modifiedTestCellPos(dim) += (dim%2==0?1:-1)*simulatedSystem.boxSize(dim);
			newCellDistance = (modifiedTestCellPos-currentCellPos).norm();
			if(newCellDistance < cellDistance)
			{
				testCellOffset(dim) += (dim%2==0?1:-1)*simulatedSystem.boxSize(dim);
				cellDistance = newCellDistance;
			}
		}
	}*/
	Scalar maxCellLength = 2*fmax(gravityCellHalfDiagonalLength[currentCellIndex], gravityCellHalfDiagonalLength[testCellIndex]);
	Scalar minimalDistance = fmax((currentCellPos-(testCellPos+testCellOffset)).norm()
		-gravityCellHalfDiagonalLength[currentCellIndex]-gravityCellHalfDiagonalLength[testCellIndex], 0.0000001);
	Scalar theta = maxCellLength / minimalDistance;
	if(theta<simulatedSystem.maximumTheta)
	{
		approximatingCellsPerGravityCell.addElement(currentCellIndex, testCellIndex);
		approximatingCellsOffsetPerGravityCell.addElement(currentCellIndex, testCellOffset);
	}
	else
	{
		if(testCellIndex >= gravityCellCount3)
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
	if(gravityCellIndexOfSpheres != NULL)
		delete[] gravityCellIndexOfSpheres;
	gravityCellIndexOfSpheres = NULL;
	quint16 count = spheres.count();
	if(count > 0)
		gravityCellIndexOfSpheres = new quint32[count];
}

void SphereCalculator::updateGravityCellData()
{
	quint32 cellIndex;
	for(cellIndex = 0; cellIndex<gravityAllCellCount; cellIndex++)
	{
		massVectorSumPerCell[cellIndex].setZero();
		massSumPerCell[cellIndex] = 0;
		sphereCountPerGravityCell[cellIndex] = 0;
	}
	quint32 sphereIndex;
	Sphere s;
	for(sphereIndex = 0; sphereIndex<sphCount; sphereIndex++)
	{
		s = sphArr[sphereIndex];
		cellIndex = gravityCellCount3 + gravityCellIndexOfSpheres[sphereIndex];
		massVectorSumPerCell[cellIndex] += s.mass*s.pos;
		massSumPerCell[cellIndex] += s.mass;
		sphereCountPerGravityCell[cellIndex]++;
	}
	quint32 parentCellIndex;
	for(cellIndex = gravityAllCellCount-1; cellIndex > 1; cellIndex--)
	{
		if(massSumPerCell[cellIndex] != 0)
		{
			parentCellIndex = cellIndex/2;
			massVectorSumPerCell[parentCellIndex] += massVectorSumPerCell[cellIndex];
			massSumPerCell[parentCellIndex] += massSumPerCell[cellIndex];
			
			massCenterPerCell[cellIndex] = massVectorSumPerCell[cellIndex]/massSumPerCell[cellIndex];
			sphereCountPerGravityCell[parentCellIndex] += sphereCountPerGravityCell[cellIndex];
		}
	}
}

quint16 SphereCalculator::addSphere()
{
	spheres.append();
	newSpherePos.append();
	cellIndicesOfSpheres.changeSize(spheres.count());
	collidingSpheresPerSphere.changeSize(spheres.count());
	updateGravityCellIndexOfSpheresArray();
	updateData();
	//workQueue->sendFrameData();
	return getSphereCount();
}

quint16 SphereCalculator::removeLastSphere()
{
	if(getSphereCount()>0)
	{
		return removeSphere(getSphereCount()-1);
	}else{
		return getSphereCount();
	}
}

quint16 SphereCalculator::updateSphere(quint16 i, Sphere s)
{
	if(getSphereCount()>i)
	{
		spheres[i] = s;
		updateData();
		workQueue->sendFrameData();
	}
	return getSphereCount();
}

quint16 SphereCalculator::getSphereCount()
{
	return spheres.count();
}

Sphere SphereCalculator::getAllSphereData(quint16 i)
{
	if(getSphereCount()>i)
	{
		return spheres[i];
	}else{
		return Sphere();
	}
}

quint16 SphereCalculator::addSomeSpheres(quint16 sphCount)
{
	/*spheres.insert(spheres.count()-1, sphCount, Sphere());
	newSpherePos.insert(newSpherePos.count()-1, sphCount, Vector3());
	cellIndicesOfSpheres.changeSize(spheres.count());
	collidingSpheresPerSphere.changeSize(spheres.count());
	updateGravityCellIndexOfSpheresArray();
	updateData();
	workQueue->sendFrameData();
	return getSphereCount();*/
	for(quint16 i = 0; i<sphCount; i++)
		addSphere();
	return getSphereCount();
}

quint16 SphereCalculator::removeSomeLastSpheres(quint16 sphCount)
{
	for(quint16 i = 0; i<sphCount; i++)
		removeLastSphere();
	return getSphereCount();
}

void SphereCalculator::updateSpherePositionsInBox(Scalar randomDisplacement, Scalar randomSpeed)
{
	quint16 sphereCount1D = (quint16)ceil(pow(sphCount, 1/3.0));
	
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration timepoint = now.time_since_epoch();
	std::default_random_engine generator(timepoint.count());
	std::uniform_real_distribution<Scalar> distribution(-1, 1);
	
	for(unsigned int i = 0; i<sphCount; i++)
	{
		Console::out<<"SphereCalculator: sphere "<<(i+1)<<"|"<<sphCount<<"\r";
		Sphere& s = spheres[i];
		s.pos = simulatedSystem.boxSize/2;
		s.pos(0) += simulatedSystem.boxSize(0)/sphereCount1D*((sphereCount1D-1)/2.0-(i%sphereCount1D));
		s.pos(1) += simulatedSystem.boxSize(1)/sphereCount1D*((sphereCount1D-1)/2.0-((i/sphereCount1D)%sphereCount1D));
		s.pos(2) += simulatedSystem.boxSize(2)/sphereCount1D*((sphereCount1D-1)/2.0-((i/sphereCount1D)/sphereCount1D));
		s.speed.setZero();
		for(quint8 dim = 0; dim<3; dim++)
		{
			s.pos(dim) += s.radius*randomDisplacement*distribution(generator);
			s.speed(dim) += s.radius*randomSpeed*distribution(generator);
		}
	}
	updateData();
	qDebug()<<"SphereCalculator: updateSpherePositionsInBox finished.";
}

quint16 SphereCalculator::updateAllSpheres(Sphere s)
{
	for(quint16 i = 0; i<spheres.count(); i++)
	{
		spheres[i] = s;
	}
	updateData();
	workQueue->sendFrameData();
	return getSphereCount();
}

void SphereCalculator::calculateStep()
{
	calculateSomeSteps(1);
}

void SphereCalculator::updateTimeStep(Scalar timeSt)
{
	timeStep = timeSt;
}

Scalar SphereCalculator::getTimeStep()
{
	return timeStep;
}

void SphereCalculator::updateIntegratorMethod(quint8 integrMethod)
{
	integratorMethod = integrMethod;
	
	if(integrMethod == IntegratorMethods::HeunEuler21)
	{
		qDebug()<<"SphereCalculator: activated HeunEuler21 integrator.";
		const Scalar a[4] = 	{0.0,	0.0,
								1.0,	0.0};
		const Scalar b[2] = 	{1/2.0,	1/2.0};
		const Scalar b_[2] = 	{1.0,	0.0};
		const Scalar c[2] = 	{0.0,	1.0};
		butcherTableau = ButcherTableau(2, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::BogackiShampine32)
	{
		qDebug()<<"SphereCalculator: activated BogackiShampine32 integrator.";
		const Scalar a[16] = 	{0.0,	0.0,	0.0,	0.0,
								1/2.0,	0.0,	0.0,	0.0,
								0.0,	3/4.0,	0.0,	0.0,
								2/9.0,	1/3.0,	4/9.0,	0.0};
		const Scalar b[4] = 	{2/9.0,	1/3.0,	4/9.0,	0.0};
		const Scalar b_[4] = 	{7/24.0,	1/4.0,	1/3.0,	1/8.0};
		const Scalar c[4] = 	{0.0,	1/2.0,	3/4.0,	1.0};
		butcherTableau = ButcherTableau(4, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::CashKarp54)
	{
		qDebug()<<"SphereCalculator: activated CashKarp54 integrator.";
		const Scalar a[36] = 	{0.0,			0.0,		0.0,			0.0,			0.0,			0.0,
								1/5.0,			0.0,		0.0,			0.0,			0.0,			0.0,
								3/40.0,			9/40.0,		0.0,			0.0,			0.0,			0.0,
								3/10.0,			-9/10.0,	6/5.0,			0.0,			0.0,			0.0,
								-11/54.0,		5/2.0,		-70/27.0,		35/27.0,		0.0,			0.0,
								1631/55296.0,	175/512.0,	575/13824.0,	44275/110592.0,	253/4096.0,		0.0};
		const Scalar b[6] = 	{37/378.0,		0.0,		250/621.0,		125/594.0,		0.0,			512/1771.0};
		const Scalar b_[6] = 	{2825/27648.0,	0.0,		18575/48384.0,	13525/55296.0,	277/14336.0,	1/4.0};
		const Scalar c[6] = 	{0.0,			1/5.0,		3/10.0,			3/5.0,			1.0,			7/8.0};
		butcherTableau = ButcherTableau(6, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::DormandPrince54)
	{
		qDebug()<<"SphereCalculator: activated DormandPrince54 integrator.";
		const Scalar a[49] = 	{0.0,			0.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								1/5.0,			0.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								3/40.0,			9/40.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								44/45.0,		-56/15.0,		32/9.0,			0.0,		0.0,			0.0,		0.0,
								19372/6561.0,	-25360/2187.0,	64448/6561.0,	-212/729.0,	0.0,			0.0,		0.0,
								9017/3168.0,	-355/33.0,		46732/5247.0,	49/176.0,	-5103/18656.0,	0.0,		0.0,
								35/384.0,		0.0,			500/1113.0,		125/192.0,	-2187/6784.0,	11/84.0,	0.0};
		const Scalar b[7] = 	{35/384.0,		0.0,			500/1113.0,		125/192.0,	-2187/6784.0,	11/84.0,	0.0};
		const Scalar b_[7] = 	{5179/57600.0,	0.0,			7571/16695.0,	393/640.0,	-92097/339200.0,187/2100.0,	1/40.0};
		const Scalar c[7] = 	{0.0,			1/5.0,			3/10.0,			4/5.0,		8/9.0,			1.0,		1.0};
		butcherTableau = ButcherTableau(7, a, b, b_, c);
	}else{
		qDebug()<<"SphereCalculator: activated RungeKuttaFehlberg54 integrator.";
		integratorMethod = IntegratorMethods::RungeKuttaFehlberg54;
		const Scalar a[36] = 	{0.0,			0.0,			0.0,			0.0,			0.0,		0.0,
								1/4.0,			0.0,			0.0,			0.0,			0.0,		0.0,
								3/32.0,			9/32.0,			0.0,			0.0,			0.0,		0.0,
								1932/2197.0,	-7200/2197.0,	7296/2197.0,	0.0,			0.0,		0.0,
								439/216.0,		-8.0,			3680/513.0,		-845/4104.0,	0.0,		0.0,
								-8/27.0,		2.0,			-3544/2565.0,	1859/4104.0,	-11/40.0,	0.0};
		const Scalar b[6] = 	{16/135.0,		0.0,			6656/12825.0,	28561/56430.0,	-9/50.0,	2/55.0};
		const Scalar b_[6] = 	{25/216.0,		0.0,			1408/2565.0,	2197/4104.0,	-1/5.0,		0.0};
		const Scalar c[6] = 	{0.0,			1/4.0,			3/8.0,			12/13.0,		1.0,		1/2.0};
		butcherTableau = ButcherTableau(6, a, b, b_, c);
	}
}

quint8 SphereCalculator::getIntegratorMethod()
{
	return integratorMethod;
}

quint32 SphereCalculator::popCalculationCounter()
{
	if(sphCount>0)
	{
		quint32 counter = calculationCounter/sphCount;
		calculationCounter = 0;
		return counter;
	}else{
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

bool SphereCalculator::getIsSimulating()
{
	return workQueue->getIsSimulating();
}

quint32 SphereCalculator::popStepCounter()
{
	quint32 counter = stepCounter;
	stepCounter = 0;
	return counter;
}

void SphereCalculator::updateFrameSending(bool sendFramesRegularly)
{
	workQueue->updateFrameSending(sendFramesRegularly);
}

void SphereCalculator::updateCollisionDetection(bool detectCollisions)
{
	collisionDetectionFlag = detectCollisions;
}

void SphereCalculator::updateGravityCalculation(bool calculateGravity)
{
	gravityCalculationFlag = calculateGravity;
}

void SphereCalculator::updateLennardJonesPotentialCalculation(bool calculateLennardJonesPotential)
{
	lennardJonesPotentialFlag = calculateLennardJonesPotential;
}

void SphereCalculator::updateMaximumStepDivision(quint16 maxStepDivision_)
{
	maxStepDivision = maxStepDivision_;
}

void SphereCalculator::updateMaximumStepError(Scalar maxStepError_)
{
	maxStepError = maxStepError_;
}

quint32 SphereCalculator::getLastStepCalculationTime()
{
	return lastStepCalculationTime;
}

Scalar SphereCalculator::getTotalEnergy()
{
	if(collisionDetectionFlag)
	{
		if(gravityCalculationFlag)
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<true, true, true, true>();
				else
					return getTotalEnergy_internal<true, true, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<true, true, false, true>();
				else
					return getTotalEnergy_internal<true, true, false, false>();
			}
		}
		else
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<true, false, true, true>();
				else
					return getTotalEnergy_internal<true, false, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<true, false, false, true>();
				else
					return getTotalEnergy_internal<true, false, false, false>();
			}
		}
	}
	else
	{
		if(gravityCalculationFlag)
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<false, true, true, true>();
				else
					return getTotalEnergy_internal<false, true, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<false, true, false, true>();
				else
					return getTotalEnergy_internal<false, true, false, false>();
			}
		}
		else
		{
			if(lennardJonesPotentialFlag)
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<false, false, true, true>();
				else
					return getTotalEnergy_internal<false, false, true, false>();
			}
			else
			{
				if(simulatedSystem.periodicBoundaryConditions)
					return getTotalEnergy_internal<false, false, false, true>();
				else
					return getTotalEnergy_internal<false, false, false, false>();
			}
		}
	}
}

Scalar SphereCalculator::getKineticEnergy()
{
	Scalar totalEnergy = 0.0, sphereEnergy;
	Sphere sphere;
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex)
	{
		sphere = sphArr[sphereIndex];
		sphereEnergy = 0.5*sphere.mass*sphere.speed.squaredNorm();
		totalEnergy += sphereEnergy;
	}
	return totalEnergy;
}

void SphereCalculator::updateSphereE(Scalar E_sphere)
{
	simulatedSystem.E_sphere = E_sphere;
	updateSphereSphereE();
	updateSphereWallE();
}

void SphereCalculator::updateSpherePoissonRatio(Scalar poisson_sphere)
{
	simulatedSystem.poisson_sphere = poisson_sphere;
	updateSphereSphereE();
	updateSphereWallE();
}

void SphereCalculator::updateWallE(Scalar E_wall)
{
	simulatedSystem.E_wall = E_wall;
	updateSphereWallE();
}

void SphereCalculator::updateWallPoissonRatio(Scalar poisson_wall)
{
	simulatedSystem.poisson_wall = poisson_wall;
	updateSphereWallE();
}

void SphereCalculator::updateEarthGravity(Vector3 earthGravity)
{
	simulatedSystem.earthGravity = earthGravity;
}

void SphereCalculator::updateGravitationalConstant(Scalar G)
{
	simulatedSystem.gravitationalConstant = G;
}

void SphereCalculator::updateBoxSize(Vector3 boxSize)
{
	simulatedSystem.boxSize = boxSize;
}

void SphereCalculator::updateKineticEnergy(Scalar factor)
{
	factor = sqrt(factor);
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; sphereIndex++)
	{
		sphArr[sphereIndex].speed *= factor;
	}
}

void SphereCalculator::updateTargetTemperature(Scalar targetTemperature)
{
	Scalar factor = 3*sphCount*simulatedSystem.kBoltzmann*targetTemperature/(2*getKineticEnergy());
	updateKineticEnergy(factor);
}

void SphereCalculator::updatePeriodicBoundaryConditions(bool periodicBoundaryConditions)
{
	simulatedSystem.periodicBoundaryConditions = periodicBoundaryConditions;
	rebuildGravityCellPairs();
}
