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
		runCalculationActionTests_internal(TOSTR(integratorMethod));	\
	endTest();															\
}

using namespace SphereSim;

const int ServerTester::framebuffer = 255;

ServerTester::ServerTester(QStringList args, QHostAddress addr, quint16 port){
	qDebug()<<"ServerTester: constructor called";
	sender = new ActionSender(args, addr, port);
	testCounter = 0;
	successCounter = 0;
	testSuccess = true;
}
ServerTester::ServerTester(QStringList args, QString addr, quint16 port)
	:ServerTester(args,QHostAddress(addr),port){
}
ServerTester::ServerTester(QStringList args, const char* addr, quint16 port)
	:ServerTester(args,QString(addr),port){
}

ServerTester::~ServerTester(){
	delete sender;
}

void ServerTester::runTests(){
	Console::out<<"\n";
	runTests_(ActionGroups::basic);
	runTests_(ActionGroups::spheresUpdating);
	runTests_(ActionGroups::calculation);
	runTests_(ServerTester::framebuffer);
}

void ServerTester::runTests(quint8 actionGroup, const char* groupName){
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
	case ServerTester::framebuffer:
		runFrameBufferTests();
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
		Console::bold<<"SphereSim Tester v" VERSION_STR;
		Console::out<<" (using floating type '"<<TOSTR(FLOATING_TYPE)<<"')\n";
		Console::out<<"ServerTester: ";
		Console::bold<<sender->getVersion();
		Console::out<<" (using floating type '"<<sender->getFloatingType()<<"')\n";
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
	s.pos(0) = 1.0;
	s.pos(1) = 2.0;
	s.pos(2) = 3.0;
	s.speed(0) = 4.0;
	s.speed(1) = 5.0;
	s.speed(2) = 6.0;
	s.acc(0) = 7.0;
	s.acc(1) = 8.0;
	s.acc(2) = 9.0;
	s.mass = 10.0;
	s.radius = 11.0;
	startTest_(SpheresUpdatingActions::getOne);
		Sphere sphere;
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), ApproxEqual, 0.0);
		verify(sphere.pos(1), ApproxEqual, 0.0);
		verify(sphere.pos(2), ApproxEqual, 0.0);
		verify(sphere.speed(0), ApproxEqual, 0.0);
		verify(sphere.speed(1), ApproxEqual, 0.0);
		verify(sphere.speed(2), ApproxEqual, 0.0);
		verify(sphere.acc(0), ApproxEqual, 0.0);
		verify(sphere.acc(1), ApproxEqual, 0.0);
		verify(sphere.acc(2), ApproxEqual, 0.0);
		verify(sphere.mass, ApproxEqual, 0.0);
		verify(sphere.radius, ApproxEqual, 0.0);
	startNewTest_(SpheresUpdatingActions::getOne);
		sphere = s;
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), ApproxEqual, 0.0);
		verify(sphere.pos(1), ApproxEqual, 0.0);
		verify(sphere.pos(2), ApproxEqual, 0.0);
		verify(sphere.speed(0), ApproxEqual, 4.0);
		verify(sphere.speed(1), ApproxEqual, 5.0);
		verify(sphere.speed(2), ApproxEqual, 6.0);
		verify(sphere.acc(0), ApproxEqual, 7.0);
		verify(sphere.acc(1), ApproxEqual, 8.0);
		verify(sphere.acc(2), ApproxEqual, 9.0);
		verify(sphere.mass, ApproxEqual, 10.0);
		verify(sphere.radius, ApproxEqual, 0.0);
	startNewTest_(SpheresUpdatingActions::getOneFull);
		sphere = Sphere();
		sender->getFullSphere(0, sphere);
		verify(sphere.pos(0), ApproxEqual, 0.0);
		verify(sphere.pos(1), ApproxEqual, 0.0);
		verify(sphere.pos(2), ApproxEqual, 0.0);
		verify(sphere.speed(0), ApproxEqual, 0.0);
		verify(sphere.speed(1), ApproxEqual, 0.0);
		verify(sphere.speed(2), ApproxEqual, 0.0);
		verify(sphere.acc(0), ApproxEqual, 0.0);
		verify(sphere.acc(1), ApproxEqual, 0.0);
		verify(sphere.acc(2), ApproxEqual, 0.0);
		verify(sphere.mass, ApproxEqual, 0.0);
		verify(sphere.radius, ApproxEqual, 0.0);
	startNewTest_(SpheresUpdatingActions::updateOne);
		sender->updateSphere(0, s);
		sphere = Sphere();
		sender->getSphere(0, sphere);
		verify(sphere.pos(0), ApproxEqual, 1.0);
		verify(sphere.pos(1), ApproxEqual, 2.0);
		verify(sphere.pos(2), ApproxEqual, 3.0);
		verify(sphere.speed(0), ApproxEqual, 0.0);
		verify(sphere.speed(1), ApproxEqual, 0.0);
		verify(sphere.speed(2), ApproxEqual, 0.0);
		verify(sphere.acc(0), ApproxEqual, 0.0);
		verify(sphere.acc(1), ApproxEqual, 0.0);
		verify(sphere.acc(2), ApproxEqual, 0.0);
		verify(sphere.mass, ApproxEqual, 0.0);
		verify(sphere.radius, ApproxEqual, 11);
	startNewTest_(SpheresUpdatingActions::getOneFull);
		sphere = Sphere();
		sender->getFullSphere(0, sphere);
		verify(sphere.pos(0), ApproxEqual, 1.0);
		verify(sphere.pos(1), ApproxEqual, 2.0);
		verify(sphere.pos(2), ApproxEqual, 3.0);
		verify(sphere.speed(0), ApproxEqual, 4.0);
		verify(sphere.speed(1), ApproxEqual, 5.0);
		verify(sphere.speed(2), ApproxEqual, 6.0);
		verify(sphere.acc(0), ApproxEqual, 7.0);
		verify(sphere.acc(1), ApproxEqual, 8.0);
		verify(sphere.acc(2), ApproxEqual, 9.0);
		verify(sphere.mass, ApproxEqual, 10.0);
		verify(sphere.radius, ApproxEqual, 11.0);
	endTest();
	sender->removeLastSphere();
}

void ServerTester::runCalculationActionTests(){
	Scalar timeStep = 0.01;
	startTest_(CalculationActions::setTimeStep);
		sender->setTimeStep(timeStep);
		verify(timeStep, ApproxEqual, sender->getTimeStep());
	endTest();
	sender->addSphere();
	
	sender->setSphereE(5000);
	sender->setSpherePoisson(0.5);
	sender->setWallE(5000);
	sender->setWallPoisson(0.5);
	sender->setEarthGravity(Vector3(0, -9.81, 0));
	
	runCalculationActionTests_internal_(IntegratorMethods::HeunEuler21);
	runCalculationActionTests_internal_(IntegratorMethods::BogackiShampine32);
	runCalculationActionTests_internal_(IntegratorMethods::RungeKuttaFehlberg54);
	runCalculationActionTests_internal_(IntegratorMethods::CashKarp54);
	runCalculationActionTests_internal_(IntegratorMethods::DormandPrince54);
}

void ServerTester::runCalculationActionTests_internal(const char* integratorMethod){
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
	
	if(sender->getTimeStep()>0.01){
		sender->setTimeStep(0.01);
	}
	sender->popCalculationCounter();
	quint16 expectedTurningPoints = 10, turningPoints = 0;
	quint16 stepsPerFall = 2;
	quint32 steps = stepsPerFall*expectedTurningPoints;
	quint32 stepsAtOnce = (quint32)ceil(17.0/stepsPerFall*0.01/sender->getTimeStep());
	quint32 stepTime;
	Scalar pos = s.pos(1), oldPos, beginEnergy, endEnergy;
	beginEnergy = sender->getTotalEnergy();
	Scalar gradient = 0, oldGradient;
	for(quint32 i = 0; i<steps; i++){
		stepTime = sender->calculateSomeSteps(stepsAtOnce);
		verify(stepTime, Greater, 0.0);
		sender->getFullSphere(0, s);
		oldPos = pos;
		pos = s.pos(1);
		oldGradient = gradient;
		gradient = pos-oldPos;
		if(gradient*oldGradient < 0 || oldGradient == 0){
			// turning point detected
			turningPoints++;
		}
	}
	endEnergy = sender->getTotalEnergy();
	Scalar relError = 1.0-(beginEnergy/endEnergy);
	Console::out<<"rel. error: "<<relError<<" \t";
	Scalar relErrorPerStep = 1.0-pow(beginEnergy/endEnergy, 1.0/(steps*stepsAtOnce));
	quint32 realSteps = sender->popCalculationCounter();
	Scalar integratorWorth = 5+log(steps*stepsAtOnce)-0.1*log(fabs(relError))-log(realSteps);
	Console::out<<"integrator worth: "<<integratorWorth<<" ("<<integratorMethod<<"). \t";
	verify(turningPoints, GreaterOrEqual, expectedTurningPoints*0.9);
	verify(turningPoints, SmallerOrEqual, expectedTurningPoints*1.1);
}

void ServerTester::runFrameBufferTests(){
	quint16 bufferSize = 4;
	quint16 elementsPerFrame = 4;
	FrameBuffer<quint8> buffer(bufferSize, elementsPerFrame);
	quint8 element;
	startTest_(framebuffer);
		for(quint8 i = 0; i<bufferSize+1; i++){
			for(quint8 j = 0; j<elementsPerFrame; j++){
				buffer.pushElement((quint8)(j+i*elementsPerFrame));
			}
			buffer.pushFrame();
		}
		for(quint8 i = 0; i<bufferSize; i++){
			for(quint8 j = 0; j<elementsPerFrame; j++){
				element = j+i*elementsPerFrame;
				verify(buffer.popElement(), Equal, element);
			}
			buffer.popFrame();
		}
	endTest();
	startTest_(framebuffer);
		elementsPerFrame = 5;
		buffer.setElementsPerFrame(elementsPerFrame);
		for(quint8 i = 0; i<bufferSize+1; i++){
			for(quint8 j = 0; j<elementsPerFrame; j++){
				buffer.pushElement((quint8)(j+i*elementsPerFrame));
			}
			buffer.pushFrame();
		}
		for(quint8 i = 0; i<bufferSize; i++){
			for(quint8 j = 0; j<elementsPerFrame; j++){
				element = j+i*elementsPerFrame;
				verify(buffer.popElement(), Equal, element);
			}
			buffer.popFrame();
		}
	endTest();
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
