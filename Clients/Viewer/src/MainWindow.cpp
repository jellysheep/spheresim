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
	ui->glWidget->setFrameBuffer(actSend->getFrameBuffer());
	connect(ui->startButton, SIGNAL(clicked()), actionSender, SLOT(startSimulation()));
	connect(ui->stopButton, SIGNAL(clicked()), actionSender, SLOT(stopSimulation()));
	connect(actionSender, SIGNAL(newFrameReceived()), ui->glWidget, SLOT(updateGL()));
	
	Sphere s;
	s.radius = 0.1;
	s.pos(0) = 0.5;
	s.pos(1) = 0.5;
	s.pos(2) = 0.5;
	s.speed.setZero();
	s.speed(0) = 1;
	s.acc.setZero();
	s.mass = 1;
	actionSender->addSphere();
	actionSender->updateSphere(0, s);
	actionSender->updateTimeStep(0.00001);
	actionSender->updateEarthGravity(Vector3(0, -9.81, 0));
}

MainWindow::~MainWindow()
{
	delete ui;
}
