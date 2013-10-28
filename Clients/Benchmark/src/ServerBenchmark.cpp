/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionSender.hpp>
#include <ServerBenchmark.hpp>
#include <Integrators.hpp>

#include <QtTest/QTest>
#include <QHostAddress>

using namespace SphereSim;

ServerBenchmark::ServerBenchmark(QStringList args, QHostAddress addr, quint16 port)
{
	sender = new ActionSender(args, addr, port);
}

ServerBenchmark::~ServerBenchmark()
{
	delete sender;
}

void ServerBenchmark::runBenchmark()
{
	sender->updateSphereE(5000);
	sender->updateSpherePoissonRatio(0.5);
	sender->updateWallE(5000);
	sender->updateWallPoissonRatio(0.5);
	sender->updateEarthGravity(Vector3(0, -9.81, 0));
	
	sender->addSphere();
	sender->addSphere();
	
	runBenchmark_internal(false, true);
	runBenchmark_internal(true, false);
	runBenchmark_internal(false, false);
	
	qApp->exit(0);
}

void ServerBenchmark::runBenchmark_internal(bool detectCollisions, bool calculateGravity)
{
	Console::out<<"\nServerBenchmark: simulating with collision detection "<<(detectCollisions?"on":"off")<<".\n";
	Console::out<<"ServerBenchmark: simulating with gravity calculation "<<(calculateGravity?"on":"off")<<".\n";
	
	Sphere s;
	s.pos(0) = 0.11;
	s.pos(1) = 0.11;
	s.pos(2) = 0.11;
	s.speed(0) = 0.2;
	s.speed(1) = 0.6;
	s.speed(2) = 0.0;
	s.acc(0) = 0.0;
	s.acc(1) = 0.0;
	s.acc(2) = 0.0;
	s.mass = 1.0;
	s.radius = 0.1;
	sender->updateSphere(0, s);
	s.pos(1) = 0.4;
	sender->updateSphere(1, s);
	sender->updateCollisionDetection(detectCollisions);
	sender->updateGravityCalculation(calculateGravity);
	sender->updateGravitationalConstant(1.3e-2);
	if(calculateGravity)
		sender->updateWallE(0);
	else
		sender->updateWallE(5000);
	
	Scalar timeStep = 0.0001;
	Console::out<<"ServerBenchmark: simulated seconds per step: "<<timeStep<<"\n";
	sender->updateTimeStep(timeStep);
	Scalar beginEnergy, endEnergy;
	beginEnergy = sender->getTotalEnergy();
	
	quint32 stepCounter = 0;
	QElapsedTimer timer = QElapsedTimer();
	sender->startSimulation();
	timer.start();
	quint16 numParts = 100;
	for(quint16 i = 0; i<numParts; i++)
	{
		QTest::qWait(10*1000/numParts);
		stepCounter += sender->popStepCounter();
		Console::out<<"\rServerBenchmark: progress: "<<(((i+1)*100)/numParts)<<" % ";
	}
	sender->stopSimulation();
	while(sender->getIsSimulating())
	{
		QTest::qWait(1);
	}
	quint64 elapsedTime = timer.elapsed();
	Scalar stepsPerSecond = stepCounter/(elapsedTime*0.001);
	Console::out<<"\rServerBenchmark: simulated steps per second: "<<stepsPerSecond<<"\t\t\n";
	
	Scalar simulatedSeconds = stepCounter*timeStep;
	Scalar simulatedSecondsPerSecond = simulatedSeconds/(elapsedTime*0.001);
	Console::out<<"ServerBenchmark: simulated seconds per second: "<<simulatedSecondsPerSecond<<"\n";
	
	endEnergy = sender->getTotalEnergy();
	Scalar relError = 1.0-(beginEnergy/endEnergy);
	Console::out<<"ServerBenchmark: rel. error: "<<relError<<"\n";
}
