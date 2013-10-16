/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ServerBenchmark.hpp>
#include <Connection.hpp>

#include <QCoreApplication>
#include <QHostAddress>
#include <QStringList>
#include <QTimer>

using namespace SphereSim;

/**
 * method main:
 * Creates instance of ServerBenchmark to benchmark server simulations.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();
	ServerBenchmark* svrBenchmark = new ServerBenchmark(args, QHostAddress(Connection::address), Connection::port);
	QTimer::singleShot(0, svrBenchmark, SLOT(runBenchmark()));
	return app.exec();
}
