/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionSender.hpp>
#include <SimulationGrapher.hpp>
#include <Integrators.hpp>

#include <QDebug>
#include <QHostAddress>
#include <QCoreApplication>
#include <QTimer>
#include <random>
#include <chrono>

using namespace SphereSim;

SimulationGrapher::SimulationGrapher(QStringList args, QHostAddress addr, quint16 port)
	:filename("./graphdata.txt"), file(filename), stream(&file), dataPoints(625)
{
	actionSender = new ActionSender(args, addr, port);
	dataUpdateTimer = new QTimer(this);
	dataUpdateTimer->setInterval(100);
	connect(dataUpdateTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	counter = 0;
	sphereCountSqrt = 25;
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	timeStep = 0.2;
	time = 5;
	file.open(QIODevice::WriteOnly);
	data.reserve(dataPoints);
}

SimulationGrapher::~SimulationGrapher()
{
	delete dataUpdateTimer;
	delete actionSender;
}

void SimulationGrapher::runSimulation()
{
	/*Scalar radius = 0.02;
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
	for(unsigned int i = 0; i<sphereCount; i++)
	{
		actionSender->addSphere();
		s.pos(1) = 4*radius + 3.5f*radius*(i/sphereCountSqrt);// + distribution(generator);
		s.pos(0) = 4*radius + 3.5f*radius*(i%sphereCountSqrt);// + distribution(generator);
		s.speed(0) = distribution(generator)*100;
		s.speed(1) = distribution(generator)*100;
		actionSender->updateSphere(i, s);
	}
	
	actionSender->updateGravityCalculation(true);
	actionSender->updateGravitationalConstant(1.0e-5);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateEarthGravity(Vector3(0, -9.81, 0));
	actionSender->updateSphereE(20000);
	actionSender->updateWallE(20000);
	actionSender->updateCollisionDetection(true);
	actionSender->updateFrameSending(false);*/
	
	Scalar boxLength = sphereCountSqrt/8.0*2.0e-8;
	actionSender->updateBoxSize(Vector3(boxLength, boxLength, boxLength));
	float radius = 0.01*boxLength/sphereCountSqrt*8.0;
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
	s.mass = 0.01;
	for(unsigned int i = 0; i<sphereCount; i++)
	{
		actionSender->addSphere();
		s.pos(1) = 4*radius + 3.5f*radius*(i/sphereCountSqrt) + distribution(generator);
		s.pos(0) = 4*radius + 3.5f*radius*(i%sphereCountSqrt) + distribution(generator);
		actionSender->updateSphere(i, s);
	}
	
	actionSender->updateCollisionDetection(false);
	actionSender->updateGravityCalculation(false);
	actionSender->updateGravitationalConstant(1.0e-4);
	actionSender->updateLennardJonesPotentialCalculation(true);
	actionSender->updateEarthGravity(Vector3(0,0,0));
	//actionSender->updateEarthGravity(Vector3(0,-9.81,0));
	//actionSender->updateWallE(0);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateFrameSending(false);
	
	dataUpdateTimer->start();
}

void SimulationGrapher::timerUpdate()
{
	if(!actionSender->getIsSimulating())
	{
		qDebug()<<counter++;
		if(counter > 10)
		{
			Sphere s;
			Scalar speed;
			for(quint16 i = 0; i<sphereCount; i++)
			{
				actionSender->getAllSphereData(i, s);
				speed = s.speed.norm();
				data.append(speed);
			}
		}
		if(counter > 11 + dataPoints/sphereCount)
		{
			dataUpdateTimer->stop();
			qSort(data);
			data.prepend(0);
			Scalar factor = 1.0/dataPoints;
			for(quint16 i = 0; i<dataPoints+1; i++)
			{
				stream<<(data[i]*1.0e10)<<"\t"<<(i*factor)<<"\n";
			}
			stream.flush();
			file.close();
			qApp->exit(0);
		}
		else
		{
			actionSender->calculateSomeSteps((quint32)(time/timeStep));
		}
	}
}
