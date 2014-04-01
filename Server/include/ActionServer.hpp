/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSERVER_HPP_
#define _ACTIONSERVER_HPP_

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>

class QTcpServer;

namespace SphereSim
{

    /** \brief Start server and wait for incoming connections from clients. */
    class ActionServer:private QObject
    {
        Q_OBJECT

    private:
        /** \brief Listening server object. */
        QTcpServer* server;

    public:
        /** \brief Start a QTcpServer and listen to the specified port.
         * \param args The arguments that the program was invoked with.
         * \param addr The address that the server will be listening to.
         * \param port The port that the server will be listening to. */
        ActionServer(QStringList args, const char* addr, quint16 port);

        ~ActionServer();

        ActionServer() = delete;
        ActionServer(const ActionServer&) = delete;
        ActionServer& operator=(const ActionServer&) = delete;

    public slots:
        /** \brief Get connection from client and create a new ActionReceiver. */
        void newConnection();

    };

}

#endif
