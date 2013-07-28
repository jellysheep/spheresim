/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ActionServer.hpp>
#include <ActionReceiver.hpp>

#include <QTcpServer>

using namespace SphereSim;

ActionServer::ActionServer(const QHostAddress& addr, const quint16 port){
	qDebug()<<"ActionServer: constructor called";
	server = new QTcpServer();
	connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	bool succeeded = server->listen(addr, port);
	if(succeeded){
		qDebug()<<"ActionServer: listening did succeed.";
	}else{
		qDebug()<<"ActionServer: listening did not succeed.";
	}
}
ActionServer::ActionServer(const QString& addr, const quint16 port)
	:ActionServer(QHostAddress(addr),port){
}
ActionServer::ActionServer(const char* addr, const quint16 port)
	:ActionServer(QString(addr),port){
}

void ActionServer::newConnection(){
	qDebug()<<"ActionServer: got new connection";
	QTcpSocket* socket = server->nextPendingConnection();
	new ActionReceiver(socket);
}

ActionServer::~ActionServer(){
	server->close();
	delete server;
}
