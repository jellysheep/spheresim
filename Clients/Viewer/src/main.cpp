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
#include <ui_MainWindow.h>

#include <QApplication>
#include <QString>
#include <QDebug>

using namespace SphereSim;

/**
 * method main:
 * Builds up a connection to server and opens main window.
 */

int main(int argc, char** argv){
	QApplication app(argc, argv);
	app.setStyle("fusion");
	QStringList args = app.arguments();
	
	QMainWindow qMainWindow;
	Ui::MainWindow mainWindow;
	mainWindow.setupUi(&qMainWindow);
	qMainWindow.show();
	
	ActionSender actSend(args, Connection::defaultAddress, Connection::defaultPort);
	QString version = actSend.getVersion();
	qDebug()<<version;
	QString trueStr = actSend.getTrueString();
	qDebug()<<trueStr;
	
	return app.exec();
}
