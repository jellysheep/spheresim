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

ActionSender::ActionSender(const QHostAddress& a, const quint16 p){
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
ActionSender::ActionSender(const QString& addr, const quint16 port)
	:ActionSender(QHostAddress(addr),port){
}
ActionSender::ActionSender(const char* addr, const quint16 port)
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

void ActionSender::sendAction(const unsigned char actionGroup, const unsigned char action, QByteArray& arr){
	QByteArray data;
	data.append(actionGroup);
	data.append(action);
	data.append(arr);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
	if(arr.size()<50){
		qDebug()<<"ActionSender: sending"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<arr<<Connection::endByte<<data;
	}else{
		qDebug()<<"ActionSender: sending"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<"[data]"<<Connection::endByte;
	}
}
void ActionSender::sendAction(const unsigned char actionGroup, const unsigned char action){
	QByteArray arr;
	sendAction(actionGroup, action, arr);
}

QByteArray ActionSender::sendReplyAction(const unsigned char actionGroup, const unsigned char action, QByteArray& arr){
	socket->readAll(); ///clear buffer
	sendAction(actionGroup, action, arr);
	
	QByteArray retData, data;
	int endIndex, startIndex;
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
QByteArray ActionSender::sendReplyAction(const unsigned char actionGroup, const unsigned char action){
	QByteArray arr;
	return sendReplyAction(actionGroup, action, arr);
}

const QString ActionSender::getVersion(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getVersion);
}

const QString ActionSender::getTrueString(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getTrueString);
}

bool ActionSender::isConnected(){
	socket->waitForConnected(10);
	return connectedFlag;
}

unsigned int ActionSender::addSphere(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::addOne)).toUInt();
}

unsigned int ActionSender::removeLastSphere(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::removeLast)).toUInt();
}

unsigned int ActionSender::getSphereCount(){
	return QString(sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getCount)).toUInt();
}

void ActionSender::updateSphere(unsigned int i, Sphere s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	writeFullSphere(stream, s);
	sendAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::updateOne, arr);
}

void ActionSender::getSphere(unsigned int i, Sphere& s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getOne, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readSphere(retStream, s);
}
void ActionSender::getFullSphere(unsigned int i, Sphere& s){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream<<i;
	QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating, SpheresUpdatingActions::getOneFull, arr);
	QDataStream retStream(&retArr, QIODevice::ReadOnly);
	readFullSphere(retStream, s);
}

unsigned int ActionSender::calculateStep(){
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
