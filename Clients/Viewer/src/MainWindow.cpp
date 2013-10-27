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
	connect(actionSender, SIGNAL(newFrameReceived()), SLOT(showKineticEnergy()), Qt::QueuedConnection);
	timer.start();
	
	actionSender->updateTimeStep(0.001);
	actionSender->updateEarthGravity(Vector3(0, -0.81, 0));
	actionSender->updateSphereE(20000);
	actionSender->updateWallE(20000);
	actionSender->updateFrameSending(true);
	
	//prepareSystem1();
	//prepareSystem2();
	prepareSystem3();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::prepareSystem1()
{
	float radius = 0.03;
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
	Vector3 boxSize(1,1,1);
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
	float radius = 0.02;
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
		s.pos(1) = 4*radius + 3.5f*radius*(i/8);// + distribution(generator);
		s.pos(0) = 4*radius + 3.5f*radius*(i%8);// + distribution(generator);
		actionSender->updateSphere(i, s);
	}
	
	actionSender->updateGravityCalculation(true);
	actionSender->updateGravitationalConstant(1.0e-5);
	actionSender->updateEarthGravity(Vector3(0,0,0));
	actionSender->updateTimeStep(0.02);
}

void MainWindow::showKineticEnergy()
{
	if(timer.elapsed()>2000)
	{
		timer.restart();
		qDebug()<<"                                                                           "<<actionSender->getKineticEnergy();
	}
}
