/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ActionSender.hpp>
#include <ServerBenchmark.hpp>
#include <Integrators.hpp>

#include <QTcpServer>
#include <QProcess>
#include <QtTest/QTest>

using namespace SphereSim;

ServerBenchmark::ServerBenchmark(QStringList args, QHostAddress addr, quint16 port){
	qDebug()<<"ServerBenchmark: constructor called";
	sender = new ActionSender(args, addr, port);
}
ServerBenchmark::ServerBenchmark(QStringList args, QString addr, quint16 port)
	:ServerBenchmark(args,QHostAddress(addr),port){
}
ServerBenchmark::ServerBenchmark(QStringList args, const char* addr, quint16 port)
	:ServerBenchmark(args,QString(addr),port){
}

ServerBenchmark::~ServerBenchmark(){
	delete sender;
}

void ServerBenchmark::runBenchmark(){
	sender->addSphere();
	sender->setSphereE(5000);
	sender->setSpherePoisson(0.5);
	sender->setWallE(5000);
	sender->setWallPoisson(0.5);
	sender->setEarthGravity(Vector3(0, -9.81, 0));
	
	Sphere s;
	s.pos(0) = 0.11;
	s.pos(1) = 0.11;
	s.pos(2) = 0.11;
	s.speed(0) = 0.0;
	s.speed(1) = 0.0;
	s.speed(2) = 0.0;
	s.acc(0) = 0.0;
	s.acc(1) = 0.0;
	s.acc(2) = 0.0;
	s.mass = 1.0;
	s.radius = 0.1;
	sender->updateSphere(0, s);
	
	Scalar timeStep = 100;
	Console::out<<"ServerBenchmark: simulated seconds per step: "<<timeStep<<"\n";
	sender->setTimeStep(timeStep);
	Scalar beginEnergy, endEnergy;
	beginEnergy = sender->getTotalEnergy();
	
	quint32 stepCounter = 0;
	QElapsedTimer timer = QElapsedTimer();
	sender->startSimulation();
	timer.start();
	quint16 numParts = 100;
	for(quint16 i = 0; i<numParts; i++){
		QTest::qWait(10*1000/numParts);
		stepCounter += sender->popStepCounter();
		Console::out<<"\rServerBenchmark: progress: "<<(((i+1)*100)/numParts)<<" % ";
	}
	sender->stopSimulation();
	while(sender->getIsSimulating()){
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
	
	qApp->exit(0);
}
