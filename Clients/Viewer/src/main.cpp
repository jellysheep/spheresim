/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <StartDialog.hpp>
#include <ActionSender.hpp>
#include <Connection.hpp>
#include <MainWindow.hpp>

#include <QApplication>
#include <QHostAddress>
#include <QString>
#include <QDialog>

using namespace SphereSim;

/**
 * method main:
 * Builds up a connection to server and opens main window.
 */

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setStyle("fusion");
	QStringList args = app.arguments();
	
	quint16 sphereCount = StartDialog(&app).getSphereCount();
	
	if(sphereCount>0)
	{
		MainWindow mainWindow(args, QHostAddress(Connection::address), Connection::port, sphereCount);
		mainWindow.show();
		
		return app.exec();
	}
	return 0;
}
