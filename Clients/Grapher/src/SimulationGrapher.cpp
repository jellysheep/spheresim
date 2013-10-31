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
#include <SystemCreator.hpp>

#include <QDebug>
#include <QHostAddress>
#include <QCoreApplication>
#include <QTimer>
#include <random>
#include <chrono>

using namespace SphereSim;

SimulationGrapher::SimulationGrapher(QStringList args, QHostAddress addr, quint16 port)
	:filename("./graphdata.txt"), filename2("./graphdata2.txt"), dataPoints(256), stepsToEquilibrium(20)
{
	actionSender = new ActionSender(args, addr, port);
	dataUpdateTimer = new QTimer(this);
	dataUpdateTimer->setInterval(100);
	connect(dataUpdateTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	counter = 0;
	sphereCountSqrt = 16;
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	timeStep = 1.0e-13;
	time = 50.0e-13;
	data.reserve(dataPoints);
	data2.reserve(dataPoints*(stepsToEquilibrium+1));
}

SimulationGrapher::~SimulationGrapher()
{
	delete dataUpdateTimer;
	delete actionSender;
}

void SimulationGrapher::runSimulation()
{
	SystemCreator systemCreator(actionSender);
	Scalar length = systemCreator.createArgonGasSystem(sphereCount);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateFrameSending(false);
	
	dataUpdateTimer->start();
}

void SimulationGrapher::timerUpdate()
{
	if(!actionSender->getIsSimulating())
	{
		qDebug()<<++counter;
		qDebug()<<"kin. energy:"<<actionSender->getKineticEnergy();
		/*if(counter >= 2 && counter <= 5)
		{
			actionSender->updateKineticEnergy(3.0);
			qDebug()<<"updated kin. energy:"<<actionSender->getKineticEnergy();
		}*/
		
		Sphere s;
		Scalar speed;
		for(quint16 i = 0; i<sphereCount; i++)
		{
			actionSender->getAllSphereData(i, s);
			speed = s.speed.norm();
			data2.append(speed);
			if(counter > stepsToEquilibrium)
				data.append(speed);
		}
		if(counter >= stepsToEquilibrium + dataPoints/sphereCount)
		{
			dataUpdateTimer->stop();
			
			qSort(data);
			data.prepend(0);
			Scalar factor = 1.0/dataPoints;
			QFile file(filename);
			QTextStream stream(&file);
			file.open(QIODevice::WriteOnly);
			for(quint16 i = 0; i<data.count(); i++)
			{
				stream<<data[i]<<"\t"<<(i*factor)<<"\n";
			}
			stream.flush();
			file.close();
			
			qSort(data2);
			data2.prepend(0);
			factor = 1.0/(sphereCount*(stepsToEquilibrium + dataPoints/sphereCount));
			QFile file2(filename2);
			QTextStream stream2(&file2);
			file2.open(QIODevice::WriteOnly);
			for(quint16 i = 0; i<data2.count(); i++)
			{
				stream2<<data2[i]<<"\t"<<(i*factor)<<"\n";
			}
			stream2.flush();
			file2.close();
			
			qApp->exit(0);
		}
		else
		{
			actionSender->calculateSomeSteps((quint32)(time/timeStep));
		}
	}
}
