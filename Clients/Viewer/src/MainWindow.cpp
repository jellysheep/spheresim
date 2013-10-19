/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <MainWindow.hpp>
#include <ActionSender.hpp>
#include <ui_MainWindow.h>

#include <QDebug>

using namespace SphereSim;

MainWindow::MainWindow(ActionSender* actSend, QWidget* parent):QMainWindow(parent), actionSender(actSend)
{
	qDebug()<<"MainWindow: constructor called";
	ui = new Ui::MainWindow();
	ui->setupUi(this);
	connect(ui->startButton, SIGNAL(clicked()), actionSender, SLOT(startSimulation()));
	connect(ui->stopButton, SIGNAL(clicked()), actionSender, SLOT(stopSimulation()));
	connect(actionSender, SIGNAL(newFrameReceived()), ui->glWidget, SLOT(updateGL()));
}

MainWindow::~MainWindow()
{
	delete ui;
}
