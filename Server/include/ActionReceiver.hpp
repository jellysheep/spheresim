/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONRECEIVER_HPP_
#define _ACTIONRECEIVER_HPP_

#include "Actions.hpp"
#include "SphereCalculator.hpp"

#include <QObject>
#include <nanomsg/nn.hpp>
#include <string>

namespace SphereSim
{
    class MessageTransmitter;

    /** \brief Receiver of client requests. */
    class ActionReceiver:public QObject
    {
        Q_OBJECT

    private:
        SimulatedSystem simulatedSystem;

        /** \brief Storage and calculator of sphere data. */
        SphereCalculator sphCalc;

        WorkQueue* workQueue;

    public:
        /** \brief Start a new server handling requests from the client. */
        ActionReceiver();

        /** \brief Clean up member variables. */
        ~ActionReceiver();

        ActionReceiver(const ActionReceiver&) = delete;
        ActionReceiver& operator=(const ActionReceiver&) = delete;

    public slots:
        /** \brief Process and reply to received request. */
        void processRequest(std::string data);

        void sendFrame(std::string frameToSend);

        void sendVariable(std::string variableToSend);

        void simulating(bool isSimulating);

        /** \brief Send encoded data to client.
         * \param serverStatus Server status to be sent.
         * \param data Data to be sent to client. */
        void sendReply(unsigned char serverStatus, std::string dataToSend);

        void terminateServer();

        void serverReady();

    signals:
        void sendReply(std::string reply);

    };

}

#endif /*_ACTIONRECEIVER_HPP_*/
