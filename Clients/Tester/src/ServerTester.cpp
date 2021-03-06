/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionSender.hpp"
#include "ServerTester.hpp"
#include "Integrators.hpp"
#include "SystemCreator.hpp"
#include "Object.hpp"

#include <QtTest/QTest>
#include <iostream>
#include <iomanip>

#define runTests_(x) \
    runTests(x, TOSTR(x));
#define startTest_(x) \
    startTest(TOSTR(x));
#define startNewTest_(x) \
    startNewTest(TOSTR(x));

#define runCalculationActionTests_internal_(order, integrMethod)            \
{                                                                           \
    startTest_(integratorMethod);                                           \
        sender->simulatedSystem->set(SimulationVariables::integratorMethod, \
            (unsigned int)integrMethod);                                    \
        runCalculationActionTests_internal(order, TOSTR(integrMethod));     \
    endTest();                                                              \
}

using namespace SphereSim;

const int ServerTester::framebuffer = 255;

ServerTester::ServerTester(const char* addr, unsigned short sendPort,
    unsigned short recvPort)
    :sender(new ActionSender(addr, sendPort, recvPort, this)), testCounter(0),
    successCounter(0), testSuccess(true), testActionName(), testResult(0),
    systemCreator(new SystemCreator(sender)), currentTestConsole()
{
    sender->failureExitWhenDisconnected = true;
}

ServerTester::~ServerTester()
{
    delete systemCreator;
    delete sender;
}

void ServerTester::run()
{
    Console()<<'\n';
    runTests_(ActionGroups::basic);
    runTests_(ActionGroups::spheresUpdating);
    runTests_(ActionGroups::calculation);
    runTests_(ServerTester::framebuffer);
    //~ sender->terminateServer();
    qApp->exit(result());
}

void ServerTester::runTests(unsigned char actionGroup, const char* groupName)
{
    testCounter = 0;
    successCounter = 0;

    Console()<<"ServerTester: "<<Console::bold<<"Testing "<<groupName<<". \n";
    switch (actionGroup)
    {
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
        Console()<<"ServerTester: "
            <<Console::bold<<"Unknown action group requested. \n";
        break;
    }

    Console console;
    console<<"ServerTester: "<<Console::bold;
    if (testCounter == successCounter)
    {
        console<<Console::green<<"all tests passed.\n";
    }
    else
    {
        console<<Console::red<<(testCounter-successCounter)<<" out of "
            <<testCounter<<" tests failed.\n";
    }
    console<<'\n';
}

void ServerTester::runBasicActionTests()
{
    startTest_(Connection);
        verify(sender->isConnected(), Equal, true);
    endTest();
    if (sender->isConnected())
    {
        Console console;
        console<<"ServerTester: ";
        console<<Console::bold<<"SphereSim Tester v" VERSION_STR;
        console<<Console::light<<" (using floating type '"
            <<TOSTR(FLOATING_TYPE)<<"')\n";
        console<<"ServerTester: ";
        console<<Console::bold<<"SphereSim Server v";
        console<<sender->simulatedSystem->get<std::string>(
            SimulationVariables::serverVersion);
        console<<Console::light<<" (using floating type '"
            <<sender->simulatedSystem->get<std::string>(
            SimulationVariables::serverFloatingType)<<"')\n";
    }
    sender->simulatedSystem->set(SimulationVariables::frameSending, false);
}

void ServerTester::runSpheresUpdatingActionTests()
{
    startTest_(SpheresUpdatingActions::getSphereCount);
        verify(sender->simulatedSystem->get<unsigned int>(
            SimulationVariables::sphereCount), Equal, 0);
    startNewTest_(SpheresUpdatingActions::addSphere);
        verify(sender->addSphere(), Equal, 1);
        verify(sender->addSphere(), Equal, 2);
    startNewTest_(SpheresUpdatingActions::getSphereCount);
        verify(sender->simulatedSystem->get<unsigned int>(
            SimulationVariables::sphereCount), Equal, 2);
    startNewTest_(SpheresUpdatingActions::removeLast);
        verify(sender->removeLastSphere(), Equal, 1);
        verify(sender->addSphere(), Equal, 2);
        verify(sender->removeLastSphere(), Equal, 1);
        verify(sender->removeLastSphere(), Equal, 0);
        verify(sender->removeLastSphere(), Equal, 0);
    startNewTest_(SpheresUpdatingActions::getSphereCount);
        verify(sender->simulatedSystem->get<unsigned int>(
            SimulationVariables::sphereCount), Equal, 0);
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
        sender->getBasicSphereData(0, sphere);
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
        sender->getBasicSphereData(0, sphere);
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
        sender->getAllSphereData(0, sphere);
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
    startNewTest_(SpheresUpdatingActions::updateSphere);
        sender->updateSphere(0, s);
        sphere = Sphere();
        sender->getBasicSphereData(0, sphere);
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
        sender->getAllSphereData(0, sphere);
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

void ServerTester::runCalculationActionTests()
{
    Scalar timeStep = 0.01;
    startTest_(CalculationActions::updateTimeStep);
        sender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
        verify(timeStep, ApproxEqual, sender->simulatedSystem->get<Scalar>(
            SimulationVariables::timeStep));
    endTest();

    //runCalculationActionTests_internal_(2, IntegratorMethods::HeunEuler21);
    runCalculationActionTests_internal_(6, IntegratorMethods::CashKarp54);
    runCalculationActionTests_internal_(6, IntegratorMethods::RungeKuttaFehlberg54);
    runCalculationActionTests_internal_(7, IntegratorMethods::DormandPrince54);
    runCalculationActionTests_internal_(4, IntegratorMethods::BogackiShampine32);

    systemCreator->createSimpleWallCollisionSystem();
    sender->simulatedSystem->set(SimulationVariables::integratorMethod,
        (unsigned int)IntegratorMethods::CashKarp54);
    startTest_(CalculationActions::startSimulation);
        sender->startSimulation();
        do
        {
            QTest::qWait(10);
        }
        while (sender->simulatedSystem->get<bool>(
            SimulationVariables::simulating) == false);
        QTest::qWait(100);
        sender->stopSimulation();
        do
        {
            QTest::qWait(10);
        }
        while (sender->simulatedSystem->get<bool>(SimulationVariables::simulating));
        unsigned int steps = sender->popCalculationCounter();
        currentTestConsole<<"real steps: "<<std::setw(6)<<steps<<". ";
        verify(steps, Greater, 0);
    endTest();
}

void ServerTester::runCalculationActionTests_internal(unsigned char order,
    const char* integratorMethod)
{
    systemCreator->createSimpleWallCollisionSystem();

    unsigned short sphereCount = sender->simulatedSystem->get<unsigned int>(
        SimulationVariables::sphereCount);
    verify(sphereCount, Equal, 1);

    sender->simulatedSystem->set(SimulationVariables::maximumStepDivision, 16u);

    Scalar timeStep = 0.02;
    sender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
    Scalar simulationTime = 1;
    unsigned int steps = (unsigned int)(simulationTime/timeStep);
    Scalar beginEnergy, endEnergy;
    beginEnergy = sender->getTotalEnergy();
    sender->calculateSomeSteps(steps);
    do
    {
        QTest::qWait(10);
    }
    while (sender->simulatedSystem->get<bool>(SimulationVariables::simulating));
    endEnergy = sender->getTotalEnergy();
    Scalar relError = 1.0-(beginEnergy/endEnergy);
    currentTestConsole<<"rel. error: "<<std::setw(14)<<relError<<". ";
    Scalar relErrorPerStep = 1.0-pow(beginEnergy/endEnergy, 1.0/steps);
    unsigned int realSteps = sender->popCalculationCounter();
    currentTestConsole<<"real steps: "<<std::setw(6)<<realSteps<<". ";
    Scalar integratorWorth = 20-0.1*log(fabs(relError))-log(realSteps);
    currentTestConsole<<"integrator worth: "<<std::setw(8)<<integratorWorth<<". ";
    verify(fabs(relError), Smaller, 0.01);
    verify(realSteps, Greater, 0);
    sender->removeLastSphere();

    systemCreator->createSimpleWallCollisionSystem();
    beginEnergy = sender->getTotalEnergy();
    realSteps /= order;
    timeStep = simulationTime/realSteps;
    sender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
    sender->simulatedSystem->set(SimulationVariables::maximumStepDivision, 0u);
    sender->calculateSomeSteps(realSteps);
    do
    {
        QTest::qWait(10);
    }
    while (sender->simulatedSystem->get<bool>(SimulationVariables::simulating));
    realSteps = sender->popCalculationCounter();
    endEnergy = sender->getTotalEnergy();
    relError = 1.0-(beginEnergy/endEnergy);
    currentTestConsole<<"adapted: "<<std::setw(14)<<relError<<" | ";
    integratorWorth = 20-0.1*log(fabs(relError))-log(realSteps);
    currentTestConsole<<std::setw(8)<<integratorWorth
        <<" ("<<integratorMethod<<"). ";
    sender->removeLastSphere();
}

void ServerTester::runFrameBufferTests()
{
    unsigned short bufferSize = 4;
    unsigned short elementsPerFrame = 4;
    FrameBuffer<unsigned char> buffer(bufferSize, elementsPerFrame);
    unsigned char element;
    startTest_(framebuffer);
        for (unsigned char i = 0; i<bufferSize; i++)
        {
            for (unsigned char j = 0; j<elementsPerFrame; j++)
            {
                buffer.pushElement((unsigned char)(j+i*elementsPerFrame));
            }
            buffer.pushFrame();
        }
        for (unsigned char i = 0; i<bufferSize-1; i++)
        {
            for (unsigned char j = 0; j<elementsPerFrame; j++)
            {
                element = j+(i+1)*elementsPerFrame;
                verify(buffer.popElement(), Equal, element);
            }
            buffer.popFrame();
        }
    endTest();
    startTest_(framebuffer);
        elementsPerFrame = 5;
        buffer.updateElementsPerFrame(elementsPerFrame);
        for (unsigned char i = 0; i<bufferSize; i++)
        {
            for (unsigned char j = 0; j<elementsPerFrame; j++)
            {
                buffer.pushElement((unsigned char)(j+i*elementsPerFrame));
            }
            buffer.pushFrame();
        }
        for (unsigned char i = 0; i<bufferSize-1; i++)
        {
            for (unsigned char j = 0; j<elementsPerFrame; j++)
            {
                element = j+(i+1)*elementsPerFrame;
                verify(buffer.popElement(), Equal, element);
            }
            buffer.popFrame();
        }
    endTest();
}

void ServerTester::startTest(const char* actionName)
{
    testSuccess = true;
    testActionName = actionName;
    currentTestConsole<<"ServerTester: ";
    currentTestConsole<<Console::bold<<"test "<<std::setw(2)<<++testCounter<<": ";
    currentTestConsole<<Console::light;
}
void ServerTester::endTest()
{
    if (testSuccess)
    {
        successCounter++;
        currentTestConsole<<Console::green<<Console::bold<<"test passed. \n";
    }
    currentTestConsole.flush();
    currentTestConsole<<Console::white<<Console::light;
}
void ServerTester::startNewTest(const char* actionName)
{
    endTest();
    startTest(actionName);
}

unsigned short ServerTester::result()
{
    return testResult;
}
