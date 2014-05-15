/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSERVER_HPP_
#define _ACTIONSERVER_HPP_

#include "Actions.hpp"

#include <nanomsg/nn.hpp>

namespace SphereSim
{
    class ActionReceiver;
    class MessageTransmitter;

    /** \brief Start server and wait for incoming connections from clients. */
    class ActionServer
    {
    private:
        nn::socket sendSocket;
        nn::socket recvSocket;

        ActionReceiver* actionReceiver;

        MessageTransmitter* messageTransmitter;

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

    };

}

#endif /*_ACTIONSERVER_HPP_*/
