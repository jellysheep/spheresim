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
#include <QCoreApplication>

using namespace SphereSim;

ActionSender::ActionSender(QStringList args, QHostAddress a, quint16 p):frameBuffer(60){
	qDebug()<<"ActionSender: constructor called";
	addr = new QHostAddress(a);
	port = p;
	connectedFlag = false;
	createdOwnServer = false;
	receivedServerReply = false;
	lastServerStatus = ServerStatusReplies::acknowledge;
	socket = new QTcpSocket();
	connect(socket, SIGNAL(connected()), SLOT(connected()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
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
ActionSender::ActionSender(QStringList args, QString addr, quint16 port)
	:ActionSender(args,QHostAddress(addr),port){
}
ActionSender::ActionSender(QStringList args, const char* addr, quint16 port)
	:ActionSender(args,QString(addr),port){
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
	sendAction(actionGroup, action, arr);
	while(!receivedServerReply){
		QCoreApplication::processEvents();
	}
	receivedServerReply = false;
	return lastServerReplyData;
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action){
	QByteArray arr;
	return sendReplyAction(actionGroup, action, arr);
}

void ActionSender::readData(){
	QByteArray arr = socket->readAll();
	processData(arr);
}

void ActionSender::processData(QByteArray byteArray){
	qint16 endIndex, startIndex;
	endIndex = byteArray.indexOf(Connection::endByte);
	startIndex = byteArray.indexOf(Connection::startByte);
	
	if(endIndex<0){
		if(startIndex<0){
			///no endByte or startByte
			if(collectingReplyData){
				replyData.append(byteArray);
			}
		}else{
			///only startByte
			if(!collectingReplyData){
				//what if last reply did not end correctly? next reply would be skipped (waiting for endByte)...
				collectingReplyData = true;
				replyData = byteArray.right(byteArray.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0){
			///only endByte
			if(collectingReplyData){
				replyData.append(byteArray.left(endIndex));
				collectingReplyData = false;
				processReply();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex){
				///startByte before endByte
				replyData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
				collectingReplyData = false;
				processReply();
				processData(byteArray.right(byteArray.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingReplyData){
					replyData.append(byteArray.left(endIndex));
					collectingReplyData = false;
					processReply();
					processData(byteArray.right(byteArray.size()-endIndex-1));
				}
			}
		}
	}
}

void ActionSender::processReply(){
	QByteArray data = QByteArray::fromBase64(replyData);
	lastServerStatus = data[0];
	lastServerReplyData = data.right(data.length()-1);
	receivedServerReply = true;
}

void ActionSender::updateSphereCount(quint16 sphereCount){
	frameBuffer.setElementsPerFrame(sphereCount);
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
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::addOne)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
}

quint16 ActionSender::removeLastSphere(){
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::removeLast)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
}

quint16 ActionSender::getSphereCount(){
	quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getCount)).toUInt();
	updateSphereCount(sphereCount);
	return sphereCount;
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

void ActionSender::calculateStep(){
	sendAction(ActionGroups::calculation, CalculationActions::doOneStep);
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

quint32 ActionSender::popStepCounter(){
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::popStepCounter);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	quint32 stepCounter;
	retStream>>stepCounter;
	return stepCounter;
}

void ActionSender::calculateSomeSteps(quint32 steps){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<steps;
	sendAction(ActionGroups::calculation, CalculationActions::doSomeSteps, arr);
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

void ActionSender::startSimulation(){
	sendAction(ActionGroups::calculation, CalculationActions::startSimulation);
}

void ActionSender::stopSimulation(){
	sendAction(ActionGroups::calculation, CalculationActions::stopSimulation);
}

bool ActionSender::getIsSimulating(){
	QByteArray retArr = sendReplyAction(ActionGroups::calculation, CalculationActions::getIsSimulating);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	bool simulationStatus;
	retStream>>simulationStatus;
	return simulationStatus;
}
