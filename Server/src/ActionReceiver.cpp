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
#include <SphereManager.hpp>

#include <QTcpSocket>
#include <QCoreApplication>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* sock){
	collectingRequestData = false;
	socket = sock;
	connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
	sphMan = new SphereManager();
}

ActionReceiver::~ActionReceiver(){
	socket->close();
	qDebug()<<"ActionReceiver: disconnected";
	delete socket;
	delete sphMan;
}

void ActionReceiver::readData(){
	QByteArray arr = socket->readAll();
	processData(arr);
}

void ActionReceiver::processData(QByteArray byteArray){
	int endIndex, startIndex;
	endIndex = byteArray.indexOf(Connection::endByte);
	startIndex = byteArray.indexOf(Connection::startByte);
	
	if(endIndex<0){
		if(startIndex<0){
			///no endByte or startByte
			if(collectingRequestData){
				qDebug()<<"ActionReceiver: appending data to byteArrayay";
				requestData.append(byteArray);
			}
		}else{
			///only startByte
			if(!collectingRequestData){
				qDebug()<<"ActionReceiver: creating new data byteArrayay";
				//what if last request did not end correctly? next request would be skipped (waiting for endByte)...
				collectingRequestData = true;
				requestData = byteArray.right(byteArray.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0){
			///only endByte
			if(collectingRequestData){
				qDebug()<<"ActionReceiver: appending data and finishing byteArrayay";
				requestData.append(byteArray.left(endIndex));
				collectingRequestData = false;
				processRequest();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex){
				///startByte before endByte
				qDebug()<<"ActionReceiver: creating and finishing new data byteArrayay";
				requestData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
				collectingRequestData = false;
				processRequest();
				processData(byteArray.right(byteArray.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingRequestData){
					qDebug()<<"ActionReceiver: finishing and creating new data byteArrayay";
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
	char actionGroup = data[0];
	char action = data[1];
	if(data.size()>2){
		data = data.right(data.length()-2);
	}else{
		data.clear();
	}
	if(data.size()<50){
		qDebug()<<"ActionReceiver: receiving"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<data<<Connection::endByte;
	}else{
		qDebug()<<"ActionReceiver: receiving"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<"[data]"<<Connection::endByte;
	}
	handleBasicAction(actionGroup, action, data);
}

void ActionReceiver::handleAction(const char actionGroup, const char action, const QByteArray data){
	switch(actionGroup){
	case ActionGroups::basic:
		handleBasicAction(actionGroup, action, data);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleBasicAction(const char actionGroup, const char action, const QByteArray data){
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
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleUnknownAction(const char actionGroup, const char action, const QByteArray data){
	qWarning()<<"ActionReceiver: Warning: received unknown action group or action"
		<<Connection::startByte<<(int)actionGroup<<(int)action<<Connection::endByte;
	sendReply("unknown");
}

void ActionReceiver::sendReply(const QByteArray& arr){
	QByteArray data = arr.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
	if(arr.size()<50){
		qDebug()<<"ActionReceiver: sending"<<Connection::startByte<<arr<<Connection::endByte<<data;
	}else{
		qDebug()<<"ActionReceiver: sending"<<Connection::startByte<<"[data]"<<Connection::endByte;
	}
}
