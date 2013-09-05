/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ActionReceiver.hpp>
#include <Version.hpp>
#include <Connection.hpp>
#include <SphereTransmit.hpp>

#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* sock):sphMan(),sphCalc(sphMan.getSphereCalculator()){
	collectingRequestData = false;
	socket = sock;
	connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
}

ActionReceiver::~ActionReceiver(){
	socket->close();
	qDebug()<<"ActionReceiver: disconnected";
	delete socket;
}

void ActionReceiver::readData(){
	QByteArray arr = socket->readAll();
	processData(arr);
}

void ActionReceiver::processData(QByteArray byteArray){
	qint16 endIndex, startIndex;
	endIndex = byteArray.indexOf(Connection::endByte);
	startIndex = byteArray.indexOf(Connection::startByte);
	
	if(endIndex<0){
		if(startIndex<0){
			///no endByte or startByte
			if(collectingRequestData){
				requestData.append(byteArray);
			}
		}else{
			///only startByte
			if(!collectingRequestData){
				//what if last request did not end correctly? next request would be skipped (waiting for endByte)...
				collectingRequestData = true;
				requestData = byteArray.right(byteArray.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0){
			///only endByte
			if(collectingRequestData){
				requestData.append(byteArray.left(endIndex));
				collectingRequestData = false;
				processRequest();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex){
				///startByte before endByte
				requestData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
				collectingRequestData = false;
				processRequest();
				processData(byteArray.right(byteArray.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingRequestData){
					requestData.append(byteArray.left(endIndex));
					collectingRequestData = false;
					processRequest();
					processData(byteArray.right(byteArray.size()-endIndex-1));
				}
			}
		}
	}
}

void ActionReceiver::processRequest(){
	QByteArray data = QByteArray::fromBase64(requestData);
	quint8 actionGroup = data[0];
	quint8 action = data[1];
	if(data.size()>2){
		data = data.right(data.length()-2);
	}else{
		data.clear();
	}
	handleAction(actionGroup, action, data);
}

void ActionReceiver::handleAction(quint8 actionGroup, quint8 action, QByteArray data){
	switch(actionGroup){
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
	default:
		handleUnknownActionGroup(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data){
	switch(action){
	case BasicActions::getVersion:
		sendReply("SphereSim Server v" VERSION_STR);
		break;
	case BasicActions::getTrueString:
		sendReply("true");
		break;
	case BasicActions::terminateServer:
		sendReply("Server terminating...");
		qDebug()<<"Server terminating...";
		socket->close();
		delete socket;
		emit QCoreApplication::instance()->quit();
		deleteLater();
		break;
	case BasicActions::getFloatingType:
		sendReply(TOSTR(FLOATING_TYPE));
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data){
	quint16 i;
	Sphere s;
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	switch(action){
	case SpheresUpdatingActions::addOne:
		sendReply(QString::number(sphMan.addSphere()).toUtf8());
		break;
	case SpheresUpdatingActions::removeLast:
		sendReply(QString::number(sphMan.removeLastSphere()).toUtf8());
		break;
	case SpheresUpdatingActions::updateOne:
		stream>>i;
		readFullSphere(stream, s);
		sphMan.updateSphere(i, s);
		break;
	case SpheresUpdatingActions::getCount:
		sendReply(QString::number(sphMan.getCount()).toUtf8());
		break;
	case SpheresUpdatingActions::getOne:
		stream>>i;
		s = sphMan.getSphere(i);
		writeSphere(retStream, s);
		sendReply(retData);
		break;
	case SpheresUpdatingActions::getOneFull:
		stream>>i;
		s = sphMan.getSphere(i);
		writeFullSphere(retStream, s);
		sendReply(retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data){
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	Scalar s;
	quint8 integratorMethod;
	quint32 steps;
	switch(action){
	case CalculationActions::doOneStep:
		sendReply(QString::number(sphMan.calculateStep()).toUtf8());
		break;
	case CalculationActions::setTimeStep:
		stream>>s;
		sphCalc.setTimeStep(s);
		break;
	case CalculationActions::getTimeStep:
		retStream<<sphCalc.getTimeStep();
		sendReply(retData);
		break;
	case CalculationActions::setIntegratorMethod:
		stream>>integratorMethod;
		sphCalc.setIntegratorMethod(integratorMethod);
		break;
	case CalculationActions::getIntegratorMethod:
		retStream<<sphCalc.getIntegratorMethod();
		sendReply(retData);
		break;
	case CalculationActions::popCalculationCounter:
		retStream<<sphCalc.popCalculationCounter();
		sendReply(retData);
		break;
	case CalculationActions::doSomeSteps:
		stream>>steps;
		sendReply(QString::number(sphCalc.doSomeSteps(steps)).toUtf8());
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data){
	QDataStream stream(&data, QIODevice::ReadOnly);
	QByteArray retData;
	QDataStream retStream(&retData, QIODevice::WriteOnly);
	switch(action){
	case InformationActions::getTotalEnergy:
		retStream<<sphCalc.getTotalEnergy();
		sendReply(retData);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data){
	qWarning()<<"ActionReceiver: Warning: received unknown action group"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	sendReply("unknown action group");
}

void ActionReceiver::handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data){
	qWarning()<<"ActionReceiver: Warning: received unknown action"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	sendReply("unknown action");
}

void ActionReceiver::sendReply(const QByteArray& arr){
	QByteArray data = arr.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
}
