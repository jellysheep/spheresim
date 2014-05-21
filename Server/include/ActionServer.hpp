/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSERVER_HPP_
#define _ACTIONSERVER_HPP_

#include <QObject>
#include <QTimer>
#include <map>
#include <nanomsg/nn.hpp>

namespace SphereSim
{
    class ActionReceiver;
    class MessageTransmitter;

    /** \brief Start server and wait for incoming connections from clients. */
    class ActionServer:private QObject
    {
        Q_OBJECT

    private:
        nn::socket sendSocket;
        nn::socket recvSocket;

        unsigned int serverID;

        MessageTransmitter* messageTransmitter;

        typedef std::map<unsigned int, ActionReceiver*> ActionReceiverMap;
        ActionReceiverMap actionReceivers;

        QTimer heartbeatTimer;
        QTimer disconnectionTimer;

    public:
        /** \brief Start a server and listen to the specified port.
         * \param addr The address that the server will be listening to.
         * \param sendPort The port that the server will be sending to.
         * \param recvPort The port that the server will be listening to. */
        ActionServer(const char* addr,
            unsigned short sendPort, unsigned short recvPort);

        ~ActionServer();

        ActionServer() = delete;
        ActionServer(const ActionServer&) = delete;
        ActionServer& operator=(const ActionServer&) = delete;

    public slots:
        void send(unsigned int clientID, std::string reply);

        void receiveRequest(std::string request);

        void tearDown();

        void sendHeartbeat();

        void disconnectionCheck();

    };

}

#endif /*_ACTIONSERVER_HPP_*/
