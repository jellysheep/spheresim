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
#include <Connection.hpp>

#include <QCoreApplication>

using namespace SphereSim;

/*
 * method main:
 * Creates instance of ActionServer to listen to incoming client connections.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	ActionServer actSvr(Connection::defaultAddress, Connection::defaultPort);
	app.exec();
	
	return 0;
}