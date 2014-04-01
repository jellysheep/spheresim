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
#include <QFile>
#include <QTextStream>
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

        quint32 counter;

        Scalar timeStep;

        Scalar time;

        quint16 sphereCountSqrt;

        quint16 sphereCount;

        quint16 dataPoints;

        std::vector<Scalar> data;

        std::vector<Scalar> temperatures;

        quint16 stepsToEquilibrium;

        quint16 stepsBeforeMeasuring;

        quint8 graphNumber;

        SystemCreator* systemCreator;

    public:
        /** \brief Start a SimulationGrapher with the specified address and port.
         * \param args The arguments that the program was invoked with.
         * \param addr The address that the socket will be connecting to.
         * \param port The port that the socket will be connecting to. */
        SimulationGrapher(QStringList args, const char* addr, quint16 port);

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
