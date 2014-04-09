/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionServer.hpp"
#include "ActionReceiver.hpp"
#include "Console.hpp"

#include <QHostAddress>
#include <QTcpServer>
#include <QCoreApplication>

using namespace SphereSim;

ActionServer::ActionServer(const char* addr, unsigned short port)
    :server(new QTcpServer())
{
    Console()<<"ActionServer: constructor called.\n";
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    bool succeeded = server->listen(QHostAddress(addr), port);
    if (succeeded)
    {
        Console()<<"ActionServer: listening did succeed.\n";
    }
    else
    {
        Console()<<"ActionServer: listening did not succeed. Exiting.\n";
        qApp->exit(1);
    }
}

void ActionServer::newConnection()
{
    Console()<<"\nActionServer: got new connection.\n";
    QTcpSocket* socket = server->nextPendingConnection();
    new ActionReceiver(socket);
}

ActionServer::~ActionServer()
{
    server->close();
    delete server;
}
