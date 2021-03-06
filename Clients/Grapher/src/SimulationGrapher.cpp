/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionSender.hpp"
#include "SimulationGrapher.hpp"
#include "Integrators.hpp"
#include "SystemCreator.hpp"
#include "Console.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <random>
#include <chrono>

using namespace SphereSim;

SimulationGrapher::SimulationGrapher(const char* addr, unsigned short sendPort,
    unsigned short recvPort)
    :actionSender(new ActionSender(addr, sendPort, recvPort, this)),
    dataUpdateTimer(new QTimer(this)), counter(0), timeStep(1.0), time(1.0),
    sphereCountSqrt(1), sphereCount(1), dataPoints(2048), data(), temperatures(),
    stepsToEquilibrium(400), stepsBeforeMeasuring(30), graphNumber(1),
    systemCreator(new SystemCreator(actionSender))
{
    actionSender->failureExitWhenDisconnected = true;
    dataUpdateTimer->setInterval(5);
    connect(dataUpdateTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    actionSender->simulatedSystem->set(SimulationVariables::frameSending, false);
}

SimulationGrapher::~SimulationGrapher()
{
    delete dataUpdateTimer;
    delete systemCreator;
    delete actionSender;
}

void SimulationGrapher::run()
{
    Console()<<"starting simulation 1.\n";
    actionSender->popStepCounter();
    actionSender->popCalculationCounter();
    counter = 0;
    sphereCount = 1;
    timeStep = 0.0001;
    time = 0.0001;
    graphNumber = 1;

    systemCreator->createMacroscopicGravitationSystem(sphereCount);
    actionSender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
    actionSender->simulatedSystem->set(SimulationVariables::frameSending, false);
    actionSender->simulatedSystem->set(SimulationVariables::maximumStepDivision, 0u);

    dataUpdateTimer->start();
}

void SimulationGrapher::runSimulation2()
{
    Console()<<"starting simulation 2.\n";
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

    systemCreator->createArgonGasSystem(sphereCount, 473.15);
    actionSender->simulatedSystem->set(SimulationVariables::timeStep, timeStep);
    actionSender->simulatedSystem->set(SimulationVariables::frameSending, false);

    dataUpdateTimer->start();
}

void SimulationGrapher::timerUpdate()
{
    if (actionSender->simulatedSystem->get<bool>(
        SimulationVariables::simulating) == false)
    {
        ++counter;
        if (graphNumber == 1)
        {
            if (counter > 1)
            {
                unsigned int lastStepCalculationTime =
                    actionSender->getLastStepCalculationTime();
                //Console()<<"sphere count:"<<sphereCount<<".\n";
                //Console()<<"last step calculation time:"
                //    <<lastStepCalculationTime<<".\n";
                Console()<<sphereCount<<" "<<lastStepCalculationTime<<'\n';
            }
            if (counter > 100)
            {
                for (unsigned short i = 0; i<sphereCount; i++)
                {
                    actionSender->removeLastSphere();
                }
                //QTimer::singleShot(0, this, SLOT(runSimulation2()));
                qApp->exit(0);
            }
            else
            {
                actionSender->removeSomeLastSpheres(sphereCount);
                sphereCount = (unsigned short)pow(1.2, counter);
                systemCreator->createMacroscopic2DCollisionSystem(sphereCount);
                actionSender->calculateStep();
            }
        }
        else if (graphNumber == 2)
        {
            Scalar kineticEnergy = actionSender->getKineticEnergy();
            Console()<<"kin. energy:"<<kineticEnergy<<".\n";
            if (counter<10 || counter%10 == 0)
            {
                actionSender->simulatedSystem->set(
                    SimulationVariables::targetTemperature, 473.15);
                kineticEnergy = actionSender->getKineticEnergy();
                Console()<<"updated kin. energy:"<<kineticEnergy<<".\n";
            }
            Scalar temperature = 2.0/3.0*kineticEnergy/(sphereCount*1.3806504e-23);
            Console()<<"temperature:"<<temperature<<".\n";
            temperatures.push_back(temperature);

            Sphere s;
            Scalar speed;
            for (unsigned short i = 0; i<sphereCount; i++)
            {
                actionSender->getAllSphereData(i, s);
                speed = s.speed.norm();
                if (counter > stepsToEquilibrium)
                {
                    data.push_back(speed);
                }
            }
            if (counter >= stepsToEquilibrium + dataPoints/sphereCount)
            {
                dataUpdateTimer->stop();

                std::sort(data.begin(), data.end());
                Scalar factor = 1.0/dataPoints;
                QFile file("./graphdata.txt");
                QTextStream stream(&file);
                file.open(QIODevice::WriteOnly);
                stream<<0<<"\t"<<0<<'\n';
                for (unsigned short i = 0; i<data.size(); i++)
                {
                    stream<<data[i]<<"\t"<<((i+1)*factor)<<'\n';
                }
                stream.flush();
                file.close();

                QFile file3("./temperatures.txt");
                QTextStream stream3(&file3);
                file3.open(QIODevice::WriteOnly);
                for (unsigned short i = 0; i<temperatures.size(); i++)
                {
                    stream3<<(i*time/timeStep)<<"\t"<<temperatures[i]<<'\n';
                }
                stream3.flush();
                file3.close();

                qApp->exit(0);
            }
            else
            {
                actionSender->calculateSomeSteps((unsigned int)(time/timeStep));
            }
        }
    }
}
