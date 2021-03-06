/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SERVERBENCHMARK_HPP_
#define _SERVERBENCHMARK_HPP_

#include "Actions.hpp"
#include "Vector.hpp"

#include <QObject>

class ActionSender;

namespace SphereSim
{

    /** \brief Benchmark for server simulations. */
    class ServerBenchmark : public QObject
    {
        Q_OBJECT

    private:
        /** \brief Client object to communicate with server. */
        ActionSender* sender;

        /** \copydoc runBenchmark
         * \param detectCollisions Flag for collision detection.
         * \param calculateGravity Flag for gravity calculation.
         * \param calculateLennardJonesPotential Flag for gravity calculation. */
        Scalar runBenchmark_internal(bool detectCollisions, bool calculateGravity,
            bool calculateLennardJonesPotential);

        /** \brief Run a benchmark test with gravitation of many spheres. */
        Scalar runBenchmark_internal2();

        /** \brief Run a benchmark test with Lennard-Jones potential. */
        Scalar runBenchmark_internal3();

    public:
        /** \brief Start a ServerBenchmark with the specified address and port.
         * \param addr The address that the socket will be connecting to.
         * \param sendPort The port that the client will be sending to.
         * \param recvPort The port that the client will be listening to. */
        ServerBenchmark(const char* addr, unsigned short sendPort,
            unsigned short recvPort);

        ~ServerBenchmark();

        ServerBenchmark() = delete;
        ServerBenchmark(const ServerBenchmark&) = delete;
        ServerBenchmark& operator=(const ServerBenchmark&) = delete;

    public slots:
        /** \brief Run all benchmarks and output results. */
        void run();

    };

}

#endif /*_SERVERBENCHMARK_HPP_*/
