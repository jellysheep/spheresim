/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <MainWindow.hpp>
#include <ActionSender.hpp>
#include <SystemCreator.hpp>
#include <ui_MainWindow.h>

#include <random>
#include <chrono>

using namespace SphereSim;

MainWindow::MainWindow(ActionSender* actSend, QWidget* parent):QMainWindow(parent), actionSender(actSend)
{
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
	connect(ui->increaseEnergy, SIGNAL(clicked()), SLOT(increaseEnergy()));
	connect(ui->decreaseEnergy, SIGNAL(clicked()), SLOT(decreaseEnergy()));
	connect(actionSender, SIGNAL(greatFrameBufferPercentageLevelUpdate(int)), SLOT(updateTargetTemperature()), Qt::QueuedConnection);
	timer.start();
	
	updateBoxLength(1);
	actionSender->updateTimeStep(0.001);
	actionSender->updateEarthGravity(Vector3(0, -0.81, 0));
	actionSender->updateSphereE(20000);
	actionSender->updateWallE(20000);
	actionSender->updateFrameSending(true);
	
	systemToPrepare = 1;
	
	switch(systemToPrepare)
	{
	case 1:
		prepareSystem1();
		break;
	default:
		systemToPrepare = 2;
	case 2:
		prepareSystem2();
		break;
	case 3:
		prepareSystem3();
		break;
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::prepareSystem1()
{
	float radius = 0.03*boxLength;
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
	for(unsigned int i = 0; i<64; i++)
	{
		actionSender->addSphere();
		s.pos(1) = 2*radius + 3.5f*radius*(i/8) + distribution(generator);
		s.pos(0) = 2*radius + 3.5f*radius*(i%8) + distribution(generator);
		actionSender->updateSphere(i, s);
	}
}

void MainWindow::prepareSystem2()
{
	updateBoxLength(1);
	Vector3 boxSize(boxLength,boxLength,boxLength);
	Sphere s;
	s.radius = 0.03;
	s.pos = boxSize/2 + Vector3(0,0.2,0);
	s.speed = Vector3(+0.02,0,0);
	s.acc.setZero();
	s.mass = 1;
	actionSender->addSphere();
	actionSender->updateSphere(0, s);
	s.pos = boxSize/2 - Vector3(0,0.2,0);
	s.speed *= -1;
	actionSender->addSphere();
	actionSender->updateSphere(1, s);
	
	actionSender->updateGravityCalculation(true);
	actionSender->updateGravitationalConstant(3.0e-4);
	actionSender->updateEarthGravity(Vector3(0,0,0));
}

void MainWindow::prepareSystem3()
{
	SystemCreator systemCreator(actionSender);
	Scalar length = systemCreator.createArgonGasSystem(64, 473.15);
	qDebug()<<"system box length:"<<length;
	updateBoxLength(length);
	actionSender->updateTimeStep(2.0e-14);
}

void MainWindow::updateBoxLength(Scalar length)
{
	boxLength = length;
	ui->glWidget->setBoxLength(boxLength);
	actionSender->updateBoxSize(Vector3(boxLength,boxLength,boxLength));
}

void MainWindow::increaseEnergy()
{
	actionSender->updateKineticEnergy(2);
}

void MainWindow::decreaseEnergy()
{
	actionSender->updateKineticEnergy(0.5);
}

void MainWindow::updateTargetTemperature()
{
	if(systemToPrepare == 3)
	{
		if(timer.elapsed()<200)
			return;
		actionSender->updateTargetTemperature(473.15);
	}
}
