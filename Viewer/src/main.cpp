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

#include <QCoreApplication>
#include <QString>
#include <QDebug>

using namespace SphereSim;

/*
 * method main:
 * Creates instance of ActionSender to build up connection to server.
 */

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	ActionSender actSend(Connection::defaultAddress, Connection::defaultPort);
	const QString version = actSend.getVersion();
	qDebug()<<version;
	const QString trueStr = actSend.getTrueString();
	qDebug()<<trueStr;
	
	return 0;
}
