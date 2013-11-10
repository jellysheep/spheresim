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
#include <Console.hpp>

#include <QDebug>
#include <QHostAddress>
#include <QCoreApplication>
#include <QTimer>
#include <random>
#include <chrono>

using namespace SphereSim;

SimulationGrapher::SimulationGrapher(QStringList args, QHostAddress addr, quint16 port)
	:dataPoints(2048), stepsToEquilibrium(400), stepsBeforeMeasuring(30), graphNumber(1)
{
	actionSender = new ActionSender(args, addr, port);
	dataUpdateTimer = new QTimer(this);
	dataUpdateTimer->setInterval(5);
	connect(dataUpdateTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	systemCreator = new SystemCreator(actionSender);
	actionSender->updateFrameSending(false);
}

SimulationGrapher::~SimulationGrapher()
{
	delete dataUpdateTimer;
	delete systemCreator;
	delete actionSender;
}

void SimulationGrapher::runSimulation1()
{
	qDebug()<<"starting simulation 1";
	actionSender->popStepCounter();
	actionSender->popCalculationCounter();
	counter = 0;
	sphereCount = 1;
	timeStep = 0.0001;
	time = 0.0001;
	graphNumber = 1;
	
	Scalar length = systemCreator->createMacroscopicGravitationSystem(sphereCount);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateFrameSending(false);
	actionSender->updateMaximumStepDivision(0);
	
	dataUpdateTimer->start();
}

void SimulationGrapher::runSimulation2()
{
	qDebug()<<"starting simulation 2";
	actionSender->popStepCounter();
	actionSender->popCalculationCounter();
	counter = 0;
	sphereCountSqrt = 16;
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	timeStep = 2.0e-14;
	time = 5.0e-14;
	graphNumber = 2;
	data.reserve(dataPoints);
	temperatures.reserve(stepsToEquilibrium + dataPoints/sphereCount);
	
	Scalar length = systemCreator->createArgonGasSystem(sphereCount, 473.15);
	actionSender->updateTimeStep(timeStep);
	actionSender->updateFrameSending(false);
	
	dataUpdateTimer->start();
}

void SimulationGrapher::timerUpdate()
{
	if(!actionSender->getIsSimulating())
	{
		++counter;
		if(graphNumber == 1)
		{
			if(counter > 1)
			{
				quint32 lastStepCalculationTime = actionSender->getLastStepCalculationTime();
				//qDebug()<<"sphere count:"<<sphereCount;
				//qDebug()<<"last step calculation time:"<<lastStepCalculationTime;
				Console::out<<sphereCount<<" "<<lastStepCalculationTime<<"\n";
			}
			if(counter > 100)
			{
				for(quint16 i = 0; i<sphereCount; i++)
					actionSender->removeLastSphere();
				//QTimer::singleShot(0, this, SLOT(runSimulation2()));
				qApp->exit(0);
			}
			else
			{
				actionSender->removeSomeLastSpheres(sphereCount);
				sphereCount = (quint16)pow(1.2, counter);
				systemCreator->createMacroscopic2DCollisionSystem(sphereCount);
				actionSender->calculateStep();
			}
		}
		else if(graphNumber == 2)
		{
			Scalar kineticEnergy = actionSender->getKineticEnergy();
			qDebug()<<"kin. energy:"<<kineticEnergy;
			if(counter<10 || counter%10 == 0)
			{
				actionSender->updateTargetTemperature(473.15);
				kineticEnergy = actionSender->getKineticEnergy();
				qDebug()<<"updated kin. energy:"<<kineticEnergy;
			}
			Scalar temperature = 2.0/3.0*kineticEnergy/(sphereCount*1.3806504e-23);
			qDebug()<<"temperature:"<<temperature;
			temperatures.append(temperature);
			
			Sphere s;
			Scalar speed;
			for(quint16 i = 0; i<sphereCount; i++)
			{
				actionSender->getAllSphereData(i, s);
				speed = s.speed.norm();
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
}
