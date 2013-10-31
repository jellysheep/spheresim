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
	:dataPoints(256), stepsToEquilibrium(200), stepsBeforeMeasuring(30)
{
	actionSender = new ActionSender(args, addr, port);
	dataUpdateTimer = new QTimer(this);
	dataUpdateTimer->setInterval(20);
	connect(dataUpdateTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	counter = 0;
	sphereCountSqrt = 16;
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	timeStep = 2.0e-14;
	time = 5.0e-14;
	data.reserve(dataPoints);
	data2.reserve(dataPoints*(stepsToEquilibrium-stepsBeforeMeasuring+1));
	temperatures.reserve(stepsToEquilibrium + dataPoints/sphereCount);
}

SimulationGrapher::~SimulationGrapher()
{
	delete dataUpdateTimer;
	delete actionSender;
}

void SimulationGrapher::runSimulation()
{
	SystemCreator systemCreator(actionSender);
	Scalar length = systemCreator.createArgonGasSystem(sphereCount, 473.15);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateFrameSending(false);
	
	dataUpdateTimer->start();
}

void SimulationGrapher::timerUpdate()
{
	if(!actionSender->getIsSimulating())
	{
		qDebug()<<++counter;
		Scalar kineticEnergy = actionSender->getKineticEnergy();
		qDebug()<<"kin. energy:"<<kineticEnergy;
		Scalar temperature = kineticEnergy/(sphereCount*1.3806504e-23);
		qDebug()<<"temperature:"<<temperature;
		temperatures.append(temperature);
		if(counter>2 && counter%10 == 0)
		{
			actionSender->updateTargetTemperature(473.15);
			qDebug()<<"updated kin. energy:"<<actionSender->getKineticEnergy();
		}
		
		Sphere s;
		Scalar speed;
		for(quint16 i = 0; i<sphereCount; i++)
		{
			actionSender->getAllSphereData(i, s);
			speed = s.speed.norm();
			if(counter > stepsBeforeMeasuring)
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
			QFile file("./graphdata.txt");
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
			factor = 1.0/(sphereCount*(stepsToEquilibrium - stepsBeforeMeasuring + dataPoints/sphereCount));
			QFile file2("./graphdata2.txt");
			QTextStream stream2(&file2);
			file2.open(QIODevice::WriteOnly);
			for(quint16 i = 0; i<data2.count(); i++)
			{
				stream2<<data2[i]<<"\t"<<(i*factor)<<"\n";
			}
			stream2.flush();
			file2.close();
			
			QFile file3("./temperatures.txt");
			QTextStream stream3(&file3);
			file3.open(QIODevice::WriteOnly);
			for(quint16 i = 0; i<temperatures.count(); i++)
			{
				stream3<<(i*time/timeStep)<<"\t"<<temperatures[i]<<"\n";
			}
			stream3.flush();
			file3.close();
			
			qApp->exit(0);
		}
		else
		{
			actionSender->calculateSomeSteps((quint32)(time/timeStep));
		}
	}
}
