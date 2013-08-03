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

#include <QTcpServer>
#include <QProcess>

#define runTests_(x) \
	runTests(x, TOSTR(x));
#define startTest_(x) \
	startTest(TOSTR(x));
#define startNewTest_(x) \
	startNewTest(TOSTR(x));

using namespace SphereSim;

ServerTester::ServerTester(const QHostAddress& addr, const quint16 port){
	qDebug()<<"ServerTester: constructor called";
	sender = new ActionSender(addr, port);
	testCounter = 0;
	successCounter = 0;
}
ServerTester::ServerTester(const QString& addr, const quint16 port)
	:ServerTester(QHostAddress(addr),port){
}
ServerTester::ServerTester(const char* addr, const quint16 port)
	:ServerTester(QString(addr),port){
}

ServerTester::~ServerTester(){
	delete sender;
}

void ServerTester::runTests(){
	runTests_(ActionGroups::basic);
	runTests_(ActionGroups::spheresUpdating);
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
	verify(sender->getSphereCount(), Equal, 0);
	verify(sender->addSphere(), Equal, 1);
	verify(sender->addSphere(), Equal, 2);
	verify(sender->getSphereCount(), Equal, 2);
	verify(sender->removeLastSphere(), Equal, 1);
	verify(sender->addSphere(), Equal, 2);
	verify(sender->removeLastSphere(), Equal, 1);
	verify(sender->removeLastSphere(), Equal, 0);
	verify(sender->removeLastSphere(), Equal, 0);
	verify(sender->getSphereCount(), Equal, 0);
	
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
