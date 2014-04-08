/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionSender.hpp"
#include "ServerBenchmark.hpp"
#include "Integrators.hpp"
#include "SystemCreator.hpp"

#include <QtTest/QTest>
#include <QCoreApplication>

using namespace SphereSim;

ServerBenchmark::ServerBenchmark(const char* addr, unsigned short port)
    :sender(new ActionSender(addr, port, this))
{
    sender->failureExitWhenDisconnected = true;
}

ServerBenchmark::~ServerBenchmark()
{
    delete sender;
}

void ServerBenchmark::run()
{
    sender->simulatedSystem->set(SimulationVariables::sphereE, 5000.0);
    sender->simulatedSystem->set(SimulationVariables::spherePoissonRatio, 0.5);
    sender->simulatedSystem->set(SimulationVariables::wallE, 5000.0);
    sender->simulatedSystem->set(SimulationVariables::wallPoissonRatio, 0.5);
    sender->simulatedSystem->set(SimulationVariables::earthGravity,
        Vector3(0, -9.81, 0));

    //runBenchmark_internal2();

    runBenchmark_internal(false, false, true);
    //runBenchmark_internal(false, true, false);
    //runBenchmark_internal(true, false, false);
    //runBenchmark_internal(false, false, false);

    qApp->exit(0);
}

void ServerBenchmark::runBenchmark_internal(bool detectCollisions,
    bool calculateGravity, bool calculateLennardJonesPotential)
{
    Console()<<"\nServerBenchmark: simulating with collision detection "
        <<(detectCollisions?"on":"off")<<".\n";
    Console()<<"ServerBenchmark: simulating with gravity calculation "
        <<(calculateGravity?"on":"off")<<".\n";
    Console()<<"ServerBenchmark: simulating with "
        <<"Lennard-Jones potential calculation "
        <<(calculateLennardJonesPotential?"on":"off")<<".\n";

    sender->addSphere();
    sender->addSphere();

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
    sender->simulatedSystem->set(SimulationVariables::collisionDetection,
        detectCollisions);
    sender->simulatedSystem->set(SimulationVariables::gravityCalculation,
        calculateGravity);
    sender->simulatedSystem->set(SimulationVariables::gravitationalConstant,
        1.3e-3);
    sender->simulatedSystem->set(SimulationVariables::lennardJonesPotential,
        calculateLennardJonesPotential);
    if (calculateGravity)
    {
        sender->simulatedSystem->set(SimulationVariables::wallE, 0.0);
    }
    else
    {
        sender->simulatedSystem->set(SimulationVariables::wallE, 5000.0);
    }

    Scalar timeStep = 0.00001;
    Console()<<"ServerBenchmark: simulated seconds per step: "<<timeStep<<"\n";
    sender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
    Scalar beginEnergy, endEnergy;
    beginEnergy = sender->getTotalEnergy();

    unsigned int stepCounter = 0;
    QElapsedTimer timer = QElapsedTimer();
    sender->startSimulation();
    timer.start();
    unsigned short numParts = 100;
    for (unsigned short i = 0; i<numParts; i++)
    {
        QTest::qWait(10*1000/numParts);
        stepCounter += sender->popStepCounter();
        Console()<<"\rServerBenchmark: progress: "<<(((i+1)*100)/numParts)<<" % ";
    }
    sender->stopSimulation();
    while (sender->simulatedSystem->get<bool>(SimulationVariables::simulating))
    {
        QCoreApplication::processEvents();
    }
    unsigned int elapsedTime = timer.elapsed();
    Scalar stepsPerSecond = stepCounter/(elapsedTime*0.001);
    Console()<<"\rServerBenchmark: simulated steps per second: "
        <<stepsPerSecond<<"\t\t\n";

    Scalar simulatedSeconds = stepCounter*timeStep;
    Scalar simulatedSecondsPerSecond = simulatedSeconds/(elapsedTime*0.001);
    Console()<<"ServerBenchmark: simulated seconds per second: "
        <<simulatedSecondsPerSecond<<"\n";

    endEnergy = sender->getTotalEnergy();
    Scalar relError = 1.0-(beginEnergy/endEnergy);
    Console()<<"ServerBenchmark: rel. error: "<<relError<<"\n";

    sender->removeLastSphere();
    sender->removeLastSphere();
}

void ServerBenchmark::runBenchmark_internal2()
{
    unsigned short sphCount = 512;
    SystemCreator systemCreator(sender);
    systemCreator.createMacroscopicGravitationSystem(sphCount);

    Scalar timeStep = 0.01;
    Console()<<"ServerBenchmark: simulated seconds per step: "<<timeStep<<"\n";
    sender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);

    Scalar beginEnergy, endEnergy;
    beginEnergy = sender->getTotalEnergy();

    QElapsedTimer timer = QElapsedTimer();
    sender->startSimulation();
    timer.start();
    for (unsigned short i = 0; i<100; i++)
    {
        QTest::qWait(10*1000/100);
        Console()<<"\rServerBenchmark: progress: "<<(i+1)<<" % ";
    }
    sender->stopSimulation();
    while (sender->simulatedSystem->get<bool>(SimulationVariables::simulating))
    {
        QCoreApplication::processEvents();
    }
    unsigned int elapsedTime = timer.elapsed();
    unsigned int stepCounter = sender->popStepCounter();
    Scalar stepsPerSecond = stepCounter/(elapsedTime*0.001);
    Console()<<"\rServerBenchmark: simulated steps per second: "
        <<stepsPerSecond<<"\t\t\n";
    unsigned int calculationCounter = sender->popCalculationCounter();
    Scalar calculationsPerSecond = calculationCounter/(elapsedTime*0.001);
    Console()<<"\rServerBenchmark: calculations per second: "
        <<calculationsPerSecond<<"\t\t\n";

    Scalar simulatedSeconds = stepCounter*timeStep;
    Scalar simulatedSecondsPerSecond = simulatedSeconds/(elapsedTime*0.001);
    Console()<<"ServerBenchmark: simulated seconds per second: "
        <<simulatedSecondsPerSecond<<"\n";

    endEnergy = sender->getTotalEnergy();
    Scalar relError = 1.0-(beginEnergy/endEnergy);
    Console()<<"ServerBenchmark: rel. error: "<<relError<<"\n";

    for (unsigned short sphCounter = 0; sphCounter<sphCount; sphCounter++)
    {
        sender->removeLastSphere();
    }
}
