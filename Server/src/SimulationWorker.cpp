/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <SimulationWorker.hpp>
#include <SphereCalculator.hpp>
#include <WorkQueue.hpp>
#include <Console.hpp>
#include <ActionReceiver.hpp>
#include <Version.hpp>
#include <Connection.hpp>
#include <SphereTransmit.hpp>

#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>

using namespace SphereSim;

SimulationWorker::SimulationWorker(SphereCalculator* sphCalc_, 
	WorkQueue* queue_, ActionReceiver* actRcv_)
{
	sphCalc = sphCalc_;
	actRcv = actRcv_;
	running = true;
	queue = queue_;
	hasFinished = false;
}

SimulationWorker::~SimulationWorker()
{
	delete queue;
}

void SimulationWorker::work()
{
	WorkQueueItem workQueueItem;
	while(running)
	{
		workQueueItem = queue->popItem();
		handleAction(workQueueItem.actionGroup, workQueueItem.action, workQueueItem.data);
		QCoreApplication::processEvents();
	}
	hasFinished = true;
	emit finished();
}

void SimulationWorker::stop()
{
	running = false;
}

bool SimulationWorker::getHasFinished()
{
	return hasFinished;
}

void SimulationWorker::handleAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	switch(actionGroup)
	{
	case ActionGroups::basic:
		handleBasicAction(actionGroup, action, data);
		break;
	case ActionGroups::spheresUpdating:
		handleSpheresUpdatingAction(actionGroup, action, data);
		break;
	case ActionGroups::calculation:
		handleCalculationAction(actionGroup, action, data);
		break;
	case ActionGroups::information:
		handleInformationAction(actionGroup, action, data);
		break;
	case ActionGroups::simulatedSystem:
		handleSimulatedSystemAction(actionGroup, action, data);
		break;
	case ActionGroups::workQueue:
		handleWorkQueueAction(actionGroup, action, data);
		break;
	default:
		handleUnknownActionGroup(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	switch(action)
	{
	case BasicActions::getServerVersion:
		emit sendReply(ServerStatusReplies::acknowledge, "SphereSim Server v" VERSION_STR);
		break;
	case BasicActions::getTrueString:
		emit sendReply(ServerStatusReplies::acknowledge, "true");
		break;
	case BasicActions::terminateServer:
		emit sendReply(ServerStatusReplies::terminating, "Server terminating...");
		actRcv->terminateServer();
		break;
	case BasicActions::getServerFloatingType:
		emit sendReply(ServerStatusReplies::acknowledge, TOSTR(FLOATING_TYPE));
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	quint16 i;
	Sphere s;
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s1, s2;
	switch(action)
	{
	case SpheresUpdatingActions::addSphere:
		emit sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc->addSphere()).toUtf8());
		emit sendReply(ServerStatusReplies::sphereCountChanged, QString::number(sphCalc->getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::removeLastSphere:
		emit sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc->removeLastSphere()).toUtf8());
		emit sendReply(ServerStatusReplies::sphereCountChanged, QString::number(sphCalc->getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::updateSphere:
		stream>>i;
		readAllSphereData(stream, s);
		sphCalc->updateSphere(i, s);
		break;
	case SpheresUpdatingActions::getSphereCount:
		emit sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc->getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::getBasicSphereData:
		stream>>i;
		s = sphCalc->getAllSphereData(i);
		writeBasicSphereData(retStream, s);
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case SpheresUpdatingActions::getAllSphereData:
		stream>>i;
		s = sphCalc->getAllSphereData(i);
		writeAllSphereData(retStream, s);
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case SpheresUpdatingActions::addSomeSpheres:
		stream>>i;
		emit sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc->addSomeSpheres(i)).toUtf8());
		emit sendReply(ServerStatusReplies::sphereCountChanged, QString::number(sphCalc->getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::removeSomeLastSpheres:
		stream>>i;
		emit sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc->removeSomeLastSpheres(i)).toUtf8());
		emit sendReply(ServerStatusReplies::sphereCountChanged, QString::number(sphCalc->getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::updateSpherePositionsInBox:
		stream>>s1;
		stream>>s2;
		sphCalc->updateSpherePositionsInBox(s1, s2);
		break;
	case SpheresUpdatingActions::updateAllSpheres:
		readAllSphereData(stream, s);
		sphCalc->updateAllSpheres(s);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s;
	quint8 integratorMethod;
	quint32 steps;
	bool b;
	quint16 maxStepDivision;
	switch(action)
	{
	case CalculationActions::updateTimeStep:
		stream>>s;
		sphCalc->updateTimeStep(s);
		break;
	case CalculationActions::getTimeStep:
		retStream<<sphCalc->getTimeStep();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::updateIntegratorMethod:
		stream>>integratorMethod;
		sphCalc->updateIntegratorMethod(integratorMethod);
		break;
	case CalculationActions::getIntegratorMethod:
		retStream<<sphCalc->getIntegratorMethod();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::popCalculationCounter:
		retStream<<sphCalc->popCalculationCounter();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::popStepCounter:
		retStream<<sphCalc->popStepCounter();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::getIsSimulating:
		retStream<<sphCalc->getIsSimulating();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::updateFrameSending:
		stream>>b;
		sphCalc->updateFrameSending(b);
		break;
	case CalculationActions::updateCollisionDetection:
		stream>>b;
		sphCalc->updateCollisionDetection(b);
		break;
	case CalculationActions::updateGravityCalculation:
		stream>>b;
		sphCalc->updateGravityCalculation(b);
		break;
	case CalculationActions::updateLennardJonesPotentialCalculation:
		stream>>b;
		sphCalc->updateLennardJonesPotentialCalculation(b);
		break;
	case CalculationActions::updateMaximumStepDivision:
		stream>>maxStepDivision;
		sphCalc->updateMaximumStepDivision(maxStepDivision);
		break;
	case CalculationActions::updateMaximumStepError:
		stream>>s;
		sphCalc->updateMaximumStepError(s);
		break;
	case CalculationActions::getLastStepCalculationTime:
		retStream<<sphCalc->getLastStepCalculationTime();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	switch(action)
	{
	case InformationActions::getTotalEnergy:
		retStream<<sphCalc->getTotalEnergy();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case InformationActions::getKineticEnergy:
		retStream<<sphCalc->getKineticEnergy();
		emit sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleSimulatedSystemAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s, s2, s3;
	bool b;
	switch(action)
	{
	case SimulatedSystemActions::updateSphereE:
		stream>>s;
		sphCalc->updateSphereE(s);
		break;
	case SimulatedSystemActions::updateSpherePoissonRatio:
		stream>>s;
		sphCalc->updateSpherePoissonRatio(s);
		break;
	case SimulatedSystemActions::updateWallE:
		stream>>s;
		sphCalc->updateWallE(s);
		break;
	case SimulatedSystemActions::updateWallPoissonRatio:
		stream>>s;
		sphCalc->updateWallPoissonRatio(s);
		break;
	case SimulatedSystemActions::updateEarthGravity:
		stream>>s;
		stream>>s2;
		stream>>s3;
		sphCalc->updateEarthGravity(Vector3(s, s2, s3));
		break;
	case SimulatedSystemActions::updateGravitationalConstant:
		stream>>s;
		sphCalc->updateGravitationalConstant(s);
		break;
	case SimulatedSystemActions::updateBoxSize:
		stream>>s;
		stream>>s2;
		stream>>s3;
		sphCalc->updateBoxSize(Vector3(s, s2, s3));
		break;
	case SimulatedSystemActions::updateKineticEnergy:
		stream>>s;
		sphCalc->updateKineticEnergy(s);
		break;
	case SimulatedSystemActions::updateTargetTemperature:
		stream>>s;
		sphCalc->updateTargetTemperature(s);
		break;
	case SimulatedSystemActions::updatePeriodicBoundaryConditions:
		stream>>b;
		sphCalc->updatePeriodicBoundaryConditions(b);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleWorkQueueAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	switch(action)
	{
	case WorkQueueActions::stopWorker:
		stop();
		break;
	case WorkQueueActions::prepareFrameData:
		sphCalc->prepareFrameData();
		break;
	case WorkQueueActions::calculateStep:
		sphCalc->integrateRungeKuttaStep();
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void SimulationWorker::handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data)
{
	qWarning()<<"SimulationWorker: Warning: received unknown action group"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	emit sendReply(ServerStatusReplies::unknownActionGroup, "unknown action group");
}

void SimulationWorker::handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	qWarning()<<"SimulationWorker: Warning: received unknown action"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	emit sendReply(ServerStatusReplies::unknownAction, "unknown action");
}
