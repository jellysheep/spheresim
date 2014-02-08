/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionServer.hpp>
#include <Connection.hpp>

#include <QCoreApplication>
#include <QStringList>
#include <QHostAddress>

using namespace SphereSim;

/**
 * method main:
 * Creates instance of ActionServer to listen to incoming client connections.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();
	ActionServer actSvr(args, QHostAddress(Connection::listeningAddress), Connection::port);
	app.exec();
	
	return 0;
}
