/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SIMULATIONGRAPHER_HPP_
#define _SIMULATIONGRAPHER_HPP_

#include "Sphere.hpp"

#include <QtGlobal>
#include <QObject>
#include <vector>

class QTimer;

namespace SphereSim
{
    class ActionSender;
    class SystemCreator;

    /** \brief Grapher for server simulations. */
    class SimulationGrapher : public QObject
    {
        Q_OBJECT

    private:
        /** \brief Client object to communicate with server. */
        ActionSender* actionSender;

        QTimer* dataUpdateTimer;

        unsigned int counter;

        Scalar timeStep;

        Scalar time;

        unsigned short sphereCountSqrt;

        unsigned short sphereCount;

        unsigned short dataPoints;

        std::vector<Scalar> data;

        std::vector<Scalar> temperatures;

        unsigned short stepsToEquilibrium;

        unsigned short stepsBeforeMeasuring;

        unsigned char graphNumber;

        SystemCreator* systemCreator;

    public:
        /** \brief Start a SimulationGrapher with the specified address and port.
         * \param addr The address that the socket will be connecting to.
         * \param port The port that the socket will be connecting to. */
        SimulationGrapher(const char* addr, unsigned short port);

        ~SimulationGrapher();

        SimulationGrapher() = delete;
        SimulationGrapher(const SimulationGrapher&) = delete;
        SimulationGrapher& operator=(const SimulationGrapher&) = delete;

    public slots:
        /** \brief Run the first simulation and output results. */
        void run();

        /** \brief Run the second simulation and output results. */
        void runSimulation2();

        void timerUpdate();

    };

}

#endif
