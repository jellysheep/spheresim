/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ActionSender.hpp>
#include <Connection.hpp>
#include <Console.hpp>
#include <SphereTransmit.hpp>

#include <QTcpSocket>
#include <QHostAddress>
#include <QProcess>
#include <QDataStream>

using namespace SphereSim;

ActionSender::ActionSender(QHostAddress a, quint16 p){
	qDebug()<<"ActionSender: constructor called";
	addr = new QHostAddress(a);
	port = p;
	connectedFlag = false;
	createdOwnServer = false;
	socket = new QTcpSocket();
	connect(socket, SIGNAL(connected()), SLOT(connected()));
	connectionTryCount = 0;
	while(connectionTryCount<1000 && !connectedFlag){
		socket->connectToHost(*addr, port);
		connectionTryCount++;
		socket->waitForConnected(100);
		if(!connectedFlag){
			qDebug()<<"ActionSender: retrying to connect to host.\n";
			if(connectionTryCount<=1){
				qDebug()<<"ActionSender: starting Server.\n";
				process.start("SphereSim_Server");
				createdOwnServer = true;
			}
		}
	}
}
ActionSender::ActionSender(QString addr, quint16 port)
	:ActionSender(QHostAddress(addr),port){
}
ActionSender::ActionSender(const char* addr, quint16 port)
	:ActionSender(QString(addr),port){
}

ActionSender::~ActionSender(){
	if(createdOwnServer){
		sendReplyAction(ActionGroups::basic, BasicActions::terminateServer);
		process.waitForFinished(200);
		qDebug()<<"ActionSender: killing Server.\n";
		process.terminate();
		process.waitForFinished(200);
		process.kill();
	}
	socket->close();
	delete addr;
	delete socket;
}

void ActionSender::sendAction(quint8 actionGroup, quint8 action, QByteArray& arr){
	QByteArray data;
	data.append(actionGroup);
	data.append(action);
	data.append(arr);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
}
void ActionSender::sendAction(quint8 actionGroup, quint8 action){
	QByteArray arr;
	sendAction(actionGroup, action, arr);
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action, QByteArray& arr){
	socket->readAll(); ///clear buffer
	sendAction(actionGroup, action, arr);
	
	QByteArray retData, data;
	qint16 endIndex, startIndex;
	bool allDataReceived = false, dataStarted = false;
	while(!allDataReceived){
		if(!socket->waitForReadyRead(1000)){
			break;
		}
		data = socket->readAll();
		if(!dataStarted){
			startIndex = data.indexOf(Connection::startByte);
			if(startIndex>=0){
				dataStarted = true;
				endIndex = data.indexOf(Connection::endByte);
				if(endIndex>=0){
					retData = data.mid(startIndex+1, endIndex-startIndex-1);
					allDataReceived = true;
					break;
				}else{
					retData = data.right(startIndex+1);
					continue;
				}
			}
		}else{
			endIndex = data.indexOf(Connection::endByte);
			if(endIndex>=0){
				retData.append(data.left(endIndex));
				allDataReceived = true;
				break;
			}else{
				retData.append(data);
				continue;
			}
		}
	}
	retData = QByteArray::fromBase64(retData);
	return retData;
}
QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action){
	QByteArray arr;
	return sendReplyAction(actionGroup, action, arr);
}

QString ActionSender::getVersion(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getVersion);
}

QString ActionSender::getTrueString(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getTrueString);
}

QString ActionSender::getFloatingType(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getFloatingType);
}

bool ActionSender::isConnected(){
	socket->waitForConnected(10);
	return connectedFlag;
}

quint16 ActionSender::addSphere(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::addOne)).toUInt();
}

quint16 ActionSender::removeLastSphere(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::removeLast)).toUInt();
}

quint16 ActionSender::getSphereCount(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getCount)).toUInt();
}

void ActionSender::updateSphere(quint16 i, Sphere s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	writeFullSphere(stream, s);
	sendAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::updateOne, arr);
}

void ActionSender::getSphere(quint16 i, Sphere& s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getOne, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readSphere(retStream, s);
}
void ActionSender::getFullSphere(quint16 i, Sphere& s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getOneFull, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readFullSphere(retStream, s);
}

quint32 ActionSender::calculateStep(){
	return QString(sendReplyAction(ActionGroups::calculation, CalculationActions::doOneStep)).toUInt();
}

void ActionSender::setTimeStep(Scalar timeStep){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<timeStep;
	sendAction(ActionGroups::calculation, CalculationActions::setTimeStep, arr);
}

Scalar ActionSender::getTimeStep(){
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getTimeStep);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	Scalar timeStep;
	retStream>>timeStep;
	return timeStep;
}

void ActionSender::setIntegratorMethod(quint8 integratorMethod){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<integratorMethod;
	sendAction(ActionGroups::calculation, CalculationActions::setIntegratorMethod, arr);
}

quint8 ActionSender::getIntegratorMethod(){
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getIntegratorMethod);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint8 integratorMethod;
	retStream>>integratorMethod;
	return integratorMethod;
}

quint32 ActionSender::popCalculationCounter(){
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::popCalculationCounter);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint32 calculationCounter;
	retStream>>calculationCounter;
	return calculationCounter;
}

quint32 ActionSender::calculateSomeSteps(quint32 steps){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<steps;
	return QString(sendReplyAction(ActionGroups::calculation, CalculationActions::doSomeSteps, arr)).toUInt();
}

Scalar ActionSender::getTotalEnergy(){
	QByteArray retArr = sendReplyAction(ActionGroups::information, InformationActions::getTotalEnergy);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	Scalar totalEnergy;
	retStream>>totalEnergy;
	return totalEnergy;
}

void ActionSender::setSphereE(Scalar E_sphere){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<E_sphere;
	sendAction(ActionGroups::physicalConstants, PhysicalConstantsActions::setSphereE, arr);
}

void ActionSender::setSpherePoisson(Scalar poisson_sphere){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<poisson_sphere;
	sendAction(ActionGroups::physicalConstants, PhysicalConstantsActions::setSpherePoisson, arr);
}

void ActionSender::setWallE(Scalar E_wall){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<E_wall;
	sendAction(ActionGroups::physicalConstants, PhysicalConstantsActions::setWallE, arr);
}

void ActionSender::setWallPoisson(Scalar poisson_wall){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<poisson_wall;
	sendAction(ActionGroups::physicalConstants, PhysicalConstantsActions::setWallPoisson, arr);
}

void ActionSender::setEarthGravity(Vector3 earthGravity){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<earthGravity(0);
	stream<<earthGravity(1);
	stream<<earthGravity(2);
	sendAction(ActionGroups::physicalConstants, PhysicalConstantsActions::setEarthGravity, arr);
}
