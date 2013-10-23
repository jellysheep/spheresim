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
#include <random>
#include <chrono>

using namespace SphereSim;

MainWindow::MainWindow(ActionSender* actSend, QWidget* parent):QMainWindow(parent), actionSender(actSend)
{
	qDebug()<<"MainWindow: constructor called";
	ui = new Ui::MainWindow();
	ui->setupUi(this);
	ui->glWidget->setFrameBuffer(actSend->getFrameBuffer());
	connect(ui->startButton, SIGNAL(clicked()), actionSender, SLOT(startSimulation()));
	connect(ui->startButton, SIGNAL(clicked()), ui->glWidget, SLOT(startAnimation()));
	connect(ui->stopButton, SIGNAL(clicked()), actionSender, SLOT(stopSimulation()));
	connect(ui->stopButton, SIGNAL(clicked()), ui->glWidget, SLOT(stopAnimation()));
	connect(actionSender, SIGNAL(greatFrameBufferPercentageLevelUpdate(int)), ui->frameBuffer, SLOT(setValue(int)));
	connect(actionSender, SIGNAL(frameBufferPercentageLevelUpdate(int)), ui->glWidget, SLOT(updateTimerFrequency(int)));
	connect(ui->addSphere, SIGNAL(clicked()), actionSender, SLOT(addSphere()));
	connect(ui->removeSphere, SIGNAL(clicked()), actionSender, SLOT(removeLastSphere()));
	
	float radius = 0.07;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration timepoint = now.time_since_epoch();
	std::default_random_engine generator(timepoint.count());
	std::uniform_real_distribution<float> distribution(0.0f, radius/4);
	
	Sphere s;
	s.radius = 1.5*radius;
	s.pos(0) = 1.5*radius;
	s.pos(1) = 1.5*radius;
	s.pos(2) = 1.5*radius;
	s.speed.setZero();
	s.acc.setZero();
	s.mass = 1;
	actionSender->addSphere();
	actionSender->updateSphere(0, s);
	for(unsigned int i = 1; i<16; i++)
	{
		actionSender->addSphere();
		s.pos(1) = 2*radius + 3.5f*radius*(i/4) + distribution(generator);
		s.pos(0) = 2*radius + 3.5f*radius*(i%4) + distribution(generator);
		actionSender->updateSphere(i, s);
	}
	actionSender->updateTimeStep(0.0003);
	actionSender->updateEarthGravity(Vector3(0, -0.81, 0));
	actionSender->updateSphereE(20000);
	actionSender->updateWallE(20000);
	actionSender->updateFrameSending(true);
}

MainWindow::~MainWindow()
{
	delete ui;
}
