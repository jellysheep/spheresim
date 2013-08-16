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
#include <ServerTester.hpp>
#include <Integrators.hpp>

#include <QTcpServer>
#include <QProcess>

#define runTests_(x) \
	runTests(x, TOSTR(x));
#define startTest_(x) \
	startTest(TOSTR(x));
#define startNewTest_(x) \
	startNewTest(TOSTR(x));

#define runCalculationActionTests_internal_(integratorMethod){			\
	startTest_(integratorMethod);										\
		sender->setIntegratorMethod(integratorMethod);					\
		verify(sender->getIntegratorMethod(), Equal, integratorMethod);	\
		runCalculationActionTests_internal();							\
	endTest();															\
}

using namespace SphereSim;

ServerTester::ServerTester(QHostAddress addr, quint16 port){
	qDebug()<<"ServerTester: constructor called";
	sender = new ActionSender(addr, port);
	testCounter = 0;
	successCounter = 0;
	testSuccess = true;
}
ServerTester::ServerTester(QString addr, quint16 port)
	:ServerTester(QHostAddress(addr),port){
}
ServerTester::ServerTester(const char* addr, quint16 port)
	:ServerTester(QString(addr),port){
}

ServerTester::~ServerTester(){
	delete sender;
}

void ServerTester::runTests(){
	Console::out<<"\n";
	runTests_(ActionGroups::basic);
	runTests_(ActionGroups::spheresUpdating);
	runTests_(ActionGroups::calculation);
}

void ServerTester::runTests(ActionGroups::Group actionGroup, const char* groupName){
	testCounter = 0;
	successCounter = 0;
	
	Console::out<<"ServerTester: ";
	Console::bold<<"Testing "<<groupName<<". \n";
	switch(actionGroup){
	case ActionGroups::basic:
		runBasicActionTests();
		break;
	case ActionGroups::spheresUpdating:
		runSpheresUpdatingActionTests();
		break;
	case ActionGroups::calculation:
		runCalculationActionTests();
		break;
	default:
		Console::out<<"ServerTester: ";
		Console::bold<<"Unknown action group requested. \n";
		break;
	}
	
	Console::out<<"ServerTester: ";
	if(testCounter == successCounter){
		Console::greenBold<<"all "<<testCounter<<" tests passed.\n";
	}else{
		Console::redBold<<(testCounter-successCounter)<<" out of "<<testCounter<<" tests failed.\n";
	}
	Console::out<<"\n";
}

void ServerTester::runBasicActionTests(){
	startTest_(Connection);
		verify(sender->isConnected(), Equal, true);
	endTest();
	if(sender->isConnected()){
		Console::out<<"ServerTester: ";
		Console::bold<<"SphereSim Tester v" VERSION_STR<<'\n';
		Console::out<<"ServerTester: ";
		Console::bold<<sender->getVersion()<<'\n';
	}
	startTest_(BasicActions::getVersion);
		verify(sender->getVersion().length(), Greater, 0);
	startNewTest_(BasicActions::getTrueString);
		verify(sender->getTrueString(), Equal, "true");
	endTest();
	
}

void ServerTester::runSpheresUpdatingActionTests(){
	startTest_(SpheresUpdatingActions::getCount);
		verify(sender->getSphereCount(), Equal, 0);
	startNewTest_(SpheresUpdatingActions::addOne);
		verify(sender->addSphere(), Equal, 1);
		verify(sender->addSphere(), Equal, 2);
	startNewTest_(SpheresUpdatingActions::getCount);
		verify(sender->getSphereCount(), Equal, 2);
	startNewTest_(SpheresUpdatingActions::removeLast);
		verify(sender->removeLastSphere(), Equal, 1);
		verify(sender->addSphere(), Equal, 2);
		verify(sender->removeLastSphere(), Equal, 1);
		verify(sender->removeLastSphere(), Equal, 0);
		verify(sender->removeLastSphere(), Equal, 0);
	startNewTest_(SpheresUpdatingActions::getCount);
		verify(sender->getSphereCount(), Equal, 0);
	endTest();
	
	sender->addSphere();
	Sphere s;
	s.pos(0) = 1;
	s.pos(1) = 2;
	s.pos(2) = 3;
	s.speed(0) = 4;
	s.speed(1) = 5;
	s.speed(2) = 6;
	s.acc(0) = 7;
	s.acc(1) = 8;
	s.acc(2) = 9;
	s.mass = 10;
	s.radius = 11;
	startTest_(SpheresUpdatingActions::getOne);
		Sphere sphere;
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), Equal, 0);
		verify(sphere.pos(1), Equal, 0);
		verify(sphere.pos(2), Equal, 0);
		verify(sphere.speed(0), Equal, 0);
		verify(sphere.speed(1), Equal, 0);
		verify(sphere.speed(2), Equal, 0);
		verify(sphere.acc(0), Equal, 0);
		verify(sphere.acc(1), Equal, 0);
		verify(sphere.acc(2), Equal, 0);
		verify(sphere.mass, Equal, 0);
		verify(sphere.radius, Equal, 0);
	startNewTest_(SpheresUpdatingActions::getOne);
		sphere = s;
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), Equal, 0);
		verify(sphere.pos(1), Equal, 0);
		verify(sphere.pos(2), Equal, 0);
		verify(sphere.speed(0), Equal, 4);
		verify(sphere.speed(1), Equal, 5);
		verify(sphere.speed(2), Equal, 6);
		verify(sphere.acc(0), Equal, 7);
		verify(sphere.acc(1), Equal, 8);
		verify(sphere.acc(2), Equal, 9);
		verify(sphere.mass, Equal, 10);
		verify(sphere.radius, Equal, 0);
	startNewTest_(SpheresUpdatingActions::getOneFull);
		sphere = Sphere();
		sender->getFullSphere(0, sphere);
		verify(sphere.pos(0), Equal, 0);
		verify(sphere.pos(1), Equal, 0);
		verify(sphere.pos(2), Equal, 0);
		verify(sphere.speed(0), Equal, 0);
		verify(sphere.speed(1), Equal, 0);
		verify(sphere.speed(2), Equal, 0);
		verify(sphere.acc(0), Equal, 0);
		verify(sphere.acc(1), Equal, 0);
		verify(sphere.acc(2), Equal, 0);
		verify(sphere.mass, Equal, 0);
		verify(sphere.radius, Equal, 0);
	startNewTest_(SpheresUpdatingActions::updateOne);
		sender->updateSphere(0, s);
		sphere = Sphere();
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), Equal, 1);
		verify(sphere.pos(1), Equal, 2);
		verify(sphere.pos(2), Equal, 3);
		verify(sphere.speed(0), Equal, 0);
		verify(sphere.speed(1), Equal, 0);
		verify(sphere.speed(2), Equal, 0);
		verify(sphere.acc(0), Equal, 0);
		verify(sphere.acc(1), Equal, 0);
		verify(sphere.acc(2), Equal, 0);
		verify(sphere.mass, Equal, 0);
		verify(sphere.radius, Equal, 11);
	startNewTest_(SpheresUpdatingActions::getOneFull);
		sphere = Sphere();
		sender->getFullSphere(0, sphere);
		verify(sphere.pos(0), Equal, 1);
		verify(sphere.pos(1), Equal, 2);
		verify(sphere.pos(2), Equal, 3);
		verify(sphere.speed(0), Equal, 4);
		verify(sphere.speed(1), Equal, 5);
		verify(sphere.speed(2), Equal, 6);
		verify(sphere.acc(0), Equal, 7);
		verify(sphere.acc(1), Equal, 8);
		verify(sphere.acc(2), Equal, 9);
		verify(sphere.mass, Equal, 10);
		verify(sphere.radius, Equal, 11);
	endTest();
	sender->removeLastSphere();
}

void ServerTester::runCalculationActionTests(){
	Scalar timeStep = 0.0002;
	startTest_(CalculationActions::setTimeStep);
		sender->setTimeStep(timeStep);
		verify(timeStep, Equal, sender->getTimeStep());
	endTest();
	sender->addSphere();
	
	runCalculationActionTests_internal_(IntegratorMethods::EulerCauchy);
	runCalculationActionTests_internal_(IntegratorMethods::Midpoint);
	runCalculationActionTests_internal_(IntegratorMethods::RungeKutta4);
	runCalculationActionTests_internal_(IntegratorMethods::Leapfrog);
	runCalculationActionTests_internal_(IntegratorMethods::SemiImplicitEuler);
}

void ServerTester::runCalculationActionTests_internal(){
	Sphere s;
	s.pos(0) = 0.11;
	s.pos(1) = 0.11;
	s.pos(2) = 0.11;
	s.speed(0) = 0;
	s.speed(1) = 0;
	s.speed(2) = 0;
	s.acc(0) = 0;
	s.acc(1) = 0;
	s.acc(2) = 0;
	s.mass = 1;
	s.radius = 0.1;
	sender->updateSphere(0, s);
	
	quint16 expectedTurningPoints = 3, turningPoints = 0;
	quint16 stepTime;
	Scalar pos = s.pos(1), oldPos;
	Scalar gradient = 0, oldGradient;
	for(quint16 i = 650*expectedTurningPoints; i>0; i--){
		stepTime = sender->calculateStep();
		verify(stepTime, Greater, 0);
		sender->getSphere(0, s);
		oldPos = pos;
		pos = s.pos(1);
		oldGradient = gradient;
		gradient = pos-oldPos;
		if(gradient*oldGradient < 0 || oldGradient == 0){
			// turning point detected
			turningPoints++;
		}
	}
	verify(turningPoints, GreaterOrEqual, expectedTurningPoints-1);
	verify(turningPoints, SmallerOrEqual, expectedTurningPoints+1);
}

void ServerTester::startTest(const char* actionName){
	testSuccess = true;
	testActionName = actionName;
	Console::out<<"ServerTester: ";
	Console::bold<<"test "<<++testCounter<<": ";
}
void ServerTester::endTest(){
	if(testSuccess){
		successCounter++;
		Console::greenBold<<"test passed. \n";
	}
}
void ServerTester::startNewTest(const char* actionName){
	endTest();
	startTest(actionName);
}
