/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionSender.hpp>
#include <Connection.hpp>
#include <Console.hpp>
#include <SphereTransmit.hpp>

#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QCoreApplication>

using namespace SphereSim;

ActionSender::ActionSender(QStringList args, QHostAddress a, quint16 p):frameBuffer(10),framerateTimer()
{
	addr = new QHostAddress(a);
	port = p;
	connectedFlag = false;
	createdOwnServer = false;
	receivedServerReply = false;
	lastServerStatus = ServerStatusReplies::acknowledge;
	socket = new QTcpSocket();
	connect(socket, SIGNAL(connected()), SLOT(connected()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
	frameCounter = 0;
	receivedFramesPerSecond = 0;
	framerateTimer.start();
	frameBuffer.setActionSender(this);
	connect(this, SIGNAL(newFrameReceived()), SLOT(framerateEvent()));
	connectionTryCount = 0;
	while(connectionTryCount<1000 && !connectedFlag)
	{
		socket->connectToHost(*addr, port);
		connectionTryCount++;
		socket->waitForConnected(100);
		if(!connectedFlag)
		{
			qDebug()<<"ActionSender: retrying to connect to host.";
			if(connectionTryCount<=1)
			{
				qDebug()<<"ActionSender: starting Server.";
				serverProcess.start("SphereSim_Server");
				createdOwnServer = true;
			}
		}
	}
}

ActionSender::~ActionSender()
{
	if(createdOwnServer)
	{
		sendReplyAction(ActionGroups::basic, BasicActions::terminateServer);
		serverProcess.waitForFinished(200);
		qDebug()<<"ActionSender: killing Server.";
		serverProcess.terminate();
		serverProcess.waitForFinished(200);
		serverProcess.kill();
	}
	socket->close();
	delete addr;
	delete socket;
}

void ActionSender::sendAction(quint8 actionGroup, quint8 action, QByteArray& arr)
{
	QByteArray data;
	data.append(actionGroup);
	data.append(action);
	data.append(arr);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
}
void ActionSender::sendAction(quint8 actionGroup, quint8 action)
{
	QByteArray arr;
	sendAction(actionGroup, action, arr);
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action, QByteArray& arr)
{
	sendAction(actionGroup, action, arr);
	while(!receivedServerReply)
	{
		QCoreApplication::processEvents();
	}
	receivedServerReply = false;
	return lastServerReplyData;
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action)
{
	QByteArray arr;
	return sendReplyAction(actionGroup, action, arr);
}

void ActionSender::readData()
{
	QByteArray arr = socket->readAll();
	processData(arr);
}

void ActionSender::processData(QByteArray byteArray)
{
	qint16 endIndex, startIndex;
	endIndex = byteArray.indexOf(Connection::endByte);
	startIndex = byteArray.indexOf(Connection::startByte);
	
	if(endIndex<0)
	{
		if(startIndex<0)
		{
			///no endByte or startByte
			if(collectingReplyData)
			{
				replyData.append(byteArray);
			}
		}else{
			///only startByte
			if(!collectingReplyData)
			{
				//what if last reply did not end correctly? next reply would be skipped (waiting for endByte)...
				collectingReplyData = true;
				replyData = byteArray.right(byteArray.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0)
		{
			///only endByte
			if(collectingReplyData)
			{
				replyData.append(byteArray.left(endIndex));
				collectingReplyData = false;
				processReply();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex)
			{
				///startByte before endByte
				replyData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
				collectingReplyData = false;
				processReply();
				processData(byteArray.right(byteArray.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingReplyData)
				{
					replyData.append(byteArray.left(endIndex));
					collectingReplyData = false;
					processReply();
					processData(byteArray.right(byteArray.size()-endIndex-1));
				}
			}
		}
	}
}

void ActionSender::processReply()
{
	QByteArray data = QByteArray::fromBase64(replyData);
	lastServerStatus = data[0];
	data = data.right(data.length()-1);
	
	if(lastServerStatus == ServerStatusReplies::sendFrame)
	{
		QDataStream stream(&data, QIODevice::ReadOnly);
		quint16 sphCount;
		stream>>sphCount;
		frameBuffer.updateElementsPerFrame(sphCount);
		quint16 sphereIndex;
		Sphere sphere;
		while(!stream.atEnd())
		{
			stream>>sphereIndex;
			readBasicSphereData(stream, sphere);
			frameBuffer.pushElement(sphere);
		}
		frameBuffer.pushFrame();
		frameCounter++;
		emit newFrameReceived();
	}
	else
	{
		lastServerReplyData = data;
		receivedServerReply = true;
	}
}

void ActionSender::updateSphereCount(quint16 sphereCount)
{
	frameBuffer.updateElementsPerFrame(sphereCount);
}

bool ActionSender::isConnected()
{
	socket->waitForConnected(10);
	return connectedFlag;
}

FrameBuffer<Sphere>* ActionSender::getFrameBuffer()
{
	return &frameBuffer;
}

QString ActionSender::getServerVersion()
{
	return sendReplyAction(ActionGroups::basic, BasicActions::getServerVersion);
}

QString ActionSender::getTrueString()
{
	return sendReplyAction(ActionGroups::basic, BasicActions::getTrueString);
}

QString ActionSender::getServerFloatingType()
{
	return sendReplyAction(ActionGroups::basic, BasicActions::getServerFloatingType);
}

quint16 ActionSender::addSphere()
{
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::addSphere)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
}

quint16 ActionSender::removeLastSphere()
{
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::removeLastSphere)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
}

quint16 ActionSender::getSphereCount()
{
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getSphereCount)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
}

void ActionSender::updateSphere(quint16 i, Sphere s)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	writeAllSphereData(stream, s);
	sendAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::updateSphere, arr);
}

void ActionSender::getBasicSphereData(quint16 i, Sphere& s)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getBasicSphereData, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readBasicSphereData(retStream, s);
}
void ActionSender::getAllSphereData(quint16 i, Sphere& s)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getAllSphereData, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readAllSphereData(retStream, s);
}

void ActionSender::calculateStep()
{
	sendAction(ActionGroups::calculation, CalculationActions::calculateStep);
}

void ActionSender::updateTimeStep(Scalar timeStep)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<timeStep;
	sendAction(ActionGroups::calculation, CalculationActions::updateTimeStep, arr);
}

Scalar ActionSender::getTimeStep()
{
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getTimeStep);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	Scalar timeStep;
	retStream>>timeStep;
	return timeStep;
}

void ActionSender::updateIntegratorMethod(quint8 integratorMethod)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<integratorMethod;
	sendAction(ActionGroups::calculation, CalculationActions::updateIntegratorMethod, arr);
}

quint8 ActionSender::getIntegratorMethod()
{
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getIntegratorMethod);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint8 integratorMethod;
	retStream>>integratorMethod;
	return integratorMethod;
}

quint32 ActionSender::popCalculationCounter()
{
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::popCalculationCounter);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint32 calculationCounter;
	retStream>>calculationCounter;
	return calculationCounter;
}

quint32 ActionSender::popStepCounter()
{
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::popStepCounter);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint32 stepCounter;
	retStream>>stepCounter;
	return stepCounter;
}

void ActionSender::calculateSomeSteps(quint32 steps)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<steps;
	sendAction(ActionGroups::calculation, CalculationActions::calculateSomeSteps, arr);
}

Scalar ActionSender::getTotalEnergy()
{
	QByteArray retArr = sendReplyAction(ActionGroups::information, InformationActions::getTotalEnergy);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	Scalar totalEnergy;
	retStream>>totalEnergy;
	return totalEnergy;
}

Scalar ActionSender::getKineticEnergy()
{
	QByteArray retArr = sendReplyAction(ActionGroups::information, InformationActions::getKineticEnergy);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	Scalar kineticEnergy;
	retStream>>kineticEnergy;
	return kineticEnergy;
}

void ActionSender::updateSphereE(Scalar E_sphere)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<E_sphere;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateSphereE, arr);
}

void ActionSender::updateSpherePoissonRatio(Scalar poisson_sphere)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<poisson_sphere;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateSpherePoissonRatio, arr);
}

void ActionSender::updateWallE(Scalar E_wall)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<E_wall;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateWallE, arr);
}

void ActionSender::updateWallPoissonRatio(Scalar poisson_wall)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<poisson_wall;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateWallPoissonRatio, arr);
}

void ActionSender::updateEarthGravity(Vector3 earthGravity)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<earthGravity(0);
	stream<<earthGravity(1);
	stream<<earthGravity(2);
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateEarthGravity, arr);
}

void ActionSender::startSimulation()
{
	sendAction(ActionGroups::calculation, CalculationActions::startSimulation);
}

void ActionSender::stopSimulation()
{
	sendAction(ActionGroups::calculation, CalculationActions::stopSimulation);
}

bool ActionSender::getIsSimulating()
{
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getIsSimulating);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	bool simulationStatus;
	retStream>>simulationStatus;
	return simulationStatus;
}

void ActionSender::updateFrameSending(bool sendFramesRegularly)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<sendFramesRegularly;
	sendAction(ActionGroups::calculation, CalculationActions::updateFrameSending, arr);
}

void ActionSender::updateCollisionDetection(bool detectCollisions)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<detectCollisions;
	sendAction(ActionGroups::calculation, CalculationActions::updateCollisionDetection, arr);
}

void ActionSender::updateGravityCalculation(bool calculateGravity)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<calculateGravity;
	sendAction(ActionGroups::calculation, CalculationActions::updateGravityCalculation, arr);
}

void ActionSender::updateGravitationalConstant(Scalar G)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<G;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateGravitationalConstant, arr);
}

void ActionSender::updateLennardJonesPotentialCalculation(bool calculateLennardJonesPotential)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<calculateLennardJonesPotential;
	sendAction(ActionGroups::calculation, CalculationActions::updateLennardJonesPotentialCalculation, arr);
}

void ActionSender::updateBoxSize(Vector3 boxSize)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<boxSize(0);
	stream<<boxSize(1);
	stream<<boxSize(2);
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateBoxSize, arr);
}

void ActionSender::updateKineticEnergy(Scalar factor)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<factor;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateKineticEnergy, arr);
}

void ActionSender::updateTargetTemperature(Scalar targetTemperature)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<targetTemperature;
	sendAction(ActionGroups::simulatedSystem, SimulatedSystemActions::updateTargetTemperature, arr);
}

void ActionSender::framerateEvent()
{
	if(framerateTimer.elapsed()>1000)
	{
		if(frameCounter > 0)
		{
			receivedFramesPerSecond = frameCounter*1000.0/framerateTimer.elapsed();
			frameCounter = 0;
			emit framerateUpdate();
		}
		framerateTimer.restart();
	}
}
