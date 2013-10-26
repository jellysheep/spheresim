/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionReceiver.hpp>
#include <Version.hpp>
#include <Connection.hpp>
#include <SphereTransmit.hpp>

#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* sock):sphCalc()
{
	collectingRequestData = false;
	socket = sock;
	connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(&sphCalc, SIGNAL(frameToSend(QByteArray)), SLOT(sendFrame(QByteArray)));
}

ActionReceiver::~ActionReceiver()
{
	socket->close();
	qDebug()<<"ActionReceiver: disconnected";
	delete socket;
}

void ActionReceiver::readData()
{
	QByteArray arr = socket->readAll();
	processData(arr);
}

void ActionReceiver::processData(QByteArray byteArray)
{
	qint16 endIndex, startIndex;
	endIndex = byteArray.indexOf(Connection::endByte);
	startIndex = byteArray.indexOf(Connection::startByte);
	
	if(endIndex<0)
	{
		if(startIndex<0)
		{
			///no endByte or startByte
			if(collectingRequestData)
			{
				requestData.append(byteArray);
			}
		}else{
			///only startByte
			if(!collectingRequestData)
			{
				//what if last request did not end correctly? next request would be skipped (waiting for endByte)...
				collectingRequestData = true;
				requestData = byteArray.right(byteArray.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0)
		{
			///only endByte
			if(collectingRequestData)
			{
				requestData.append(byteArray.left(endIndex));
				collectingRequestData = false;
				processRequest();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex)
			{
				///startByte before endByte
				requestData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
				collectingRequestData = false;
				processRequest();
				processData(byteArray.right(byteArray.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingRequestData)
				{
					requestData.append(byteArray.left(endIndex));
					collectingRequestData = false;
					processRequest();
					processData(byteArray.right(byteArray.size()-endIndex-1));
				}
			}
		}
	}
}

void ActionReceiver::processRequest()
{
	QByteArray data = QByteArray::fromBase64(requestData);
	quint8 actionGroup = data[0];
	quint8 action = data[1];
	if(data.size()>2)
	{
		data = data.right(data.length()-2);
	}else{
		data.clear();
	}
	handleAction(actionGroup, action, data);
}

void ActionReceiver::handleAction(quint8 actionGroup, quint8 action, QByteArray data)
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
	default:
		handleUnknownActionGroup(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	switch(action)
	{
	case BasicActions::getServerVersion:
		sendReply(ServerStatusReplies::acknowledge, "SphereSim Server v" VERSION_STR);
		break;
	case BasicActions::getTrueString:
		sendReply(ServerStatusReplies::acknowledge, "true");
		break;
	case BasicActions::terminateServer:
		sendReply(ServerStatusReplies::terminating, "Server terminating...");
		qDebug()<<"Server terminating...";
		socket->close();
		delete socket;
		emit QCoreApplication::instance()->quit();
		deleteLater();
		break;
	case BasicActions::getServerFloatingType:
		sendReply(ServerStatusReplies::acknowledge, TOSTR(FLOATING_TYPE));
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	quint16 i;
	Sphere s;
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	switch(action)
	{
	case SpheresUpdatingActions::addSphere:
		sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc.addSphere()).toUtf8());
		break;
	case SpheresUpdatingActions::removeLastSphere:
		sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc.removeLastSphere()).toUtf8());
		break;
	case SpheresUpdatingActions::updateSphere:
		stream>>i;
		readAllSphereData(stream, s);
		sphCalc.updateSphere(i, s);
		break;
	case SpheresUpdatingActions::getSphereCount:
		sendReply(ServerStatusReplies::acknowledge, QString::number(sphCalc.getSphereCount()).toUtf8());
		break;
	case SpheresUpdatingActions::getBasicSphereData:
		stream>>i;
		s = sphCalc.getAllSphereData(i);
		writeBasicSphereData(retStream, s);
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case SpheresUpdatingActions::getAllSphereData:
		stream>>i;
		s = sphCalc.getAllSphereData(i);
		writeAllSphereData(retStream, s);
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s;
	quint8 integratorMethod;
	quint32 steps;
	bool b;
	switch(action)
	{
	case CalculationActions::calculateStep:
		sphCalc.calculateStep();
		break;
	case CalculationActions::updateTimeStep:
		stream>>s;
		sphCalc.updateTimeStep(s);
		break;
	case CalculationActions::getTimeStep:
		retStream<<sphCalc.getTimeStep();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::updateIntegratorMethod:
		stream>>integratorMethod;
		sphCalc.updateIntegratorMethod(integratorMethod);
		break;
	case CalculationActions::getIntegratorMethod:
		retStream<<sphCalc.getIntegratorMethod();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::popCalculationCounter:
		retStream<<sphCalc.popCalculationCounter();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::popStepCounter:
		retStream<<sphCalc.popStepCounter();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::calculateSomeSteps:
		stream>>steps;
		sphCalc.calculateSomeSteps(steps);
		break;
	case CalculationActions::startSimulation:
		sphCalc.startSimulation();
		break;
	case CalculationActions::stopSimulation:
		sphCalc.stopSimulation();
		break;
	case CalculationActions::getIsSimulating:
		retStream<<sphCalc.getIsSimulating();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	case CalculationActions::updateFrameSending:
		stream>>b;
		sphCalc.updateFrameSending(b);
		break;
	case CalculationActions::updateCollisionDetection:
		stream>>b;
		sphCalc.updateCollisionDetection(b);
		break;
	case CalculationActions::updateGravityCalculation:
		stream>>b;
		sphCalc.updateGravityCalculation(b);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	switch(action)
	{
	case InformationActions::getTotalEnergy:
		retStream<<sphCalc.getTotalEnergy();
		sendReply(ServerStatusReplies::acknowledge, retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleSimulatedSystemAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s, s2, s3;
	switch(action)
	{
	case SimulatedSystemActions::updateSphereE:
		stream>>s;
		sphCalc.updateSphereE(s);
		break;
	case SimulatedSystemActions::updateSpherePoissonRatio:
		stream>>s;
		sphCalc.updateSpherePoissonRatio(s);
		break;
	case SimulatedSystemActions::updateWallE:
		stream>>s;
		sphCalc.updateWallE(s);
		break;
	case SimulatedSystemActions::updateWallPoissonRatio:
		stream>>s;
		sphCalc.updateWallPoissonRatio(s);
		break;
	case SimulatedSystemActions::updateEarthGravity:
		stream>>s;
		stream>>s2;
		stream>>s3;
		sphCalc.updateEarthGravity(Vector3(s, s2, s3));
		break;
	case SimulatedSystemActions::updateGravitationalConstant:
		stream>>s;
		sphCalc.updateGravitationalConstant(s);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data)
{
	qWarning()<<"ActionReceiver: Warning: received unknown action group"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	sendReply(ServerStatusReplies::unknownActionGroup, "unknown action group");
}

void ActionReceiver::handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data)
{
	qWarning()<<"ActionReceiver: Warning: received unknown action"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	sendReply(ServerStatusReplies::unknownAction, "unknown action");
}

void ActionReceiver::sendReply(quint8 serverStatus, const QByteArray& arr)
{
	QByteArray data = arr;
	data.prepend(serverStatus);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
}

void ActionReceiver::sendFrame(QByteArray frameData)
{
	QByteArray data = frameData;
	data.prepend(ServerStatusReplies::sendFrame);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
}
