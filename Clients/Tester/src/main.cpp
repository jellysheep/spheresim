/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ServerTester.hpp>
#include <Connection.hpp>

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>

using namespace SphereSim;

/*
 * method main:
 * Creates instance of ServerTester to test server actions.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();
	ServerTester* svrTester = new ServerTester(args, Connection::defaultAddress, Connection::defaultPort);
	QTimer::singleShot(0, svrTester, SLOT(runTests()));
	return app.exec();
}
