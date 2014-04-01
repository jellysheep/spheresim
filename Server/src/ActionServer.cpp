/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionServer.hpp"
#include "ActionReceiver.hpp"

#include <QHostAddress>
#include <QTcpServer>
#include <QStringList>
#include <QCoreApplication>

using namespace SphereSim;

ActionServer::ActionServer(QStringList args, const char* addr, quint16 port)
    :server(new QTcpServer())
{
    qDebug()<<"ActionServer: constructor called";
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    bool succeeded = server->listen(QHostAddress(addr), port);
    if (succeeded)
    {
        qDebug()<<"ActionServer: listening did succeed.";
    }
    else
    {
        qDebug()<<"ActionServer: listening did not succeed. Exiting.";
        qApp->exit(1);
    }
}

void ActionServer::newConnection()
{
    qDebug()<<"\nActionServer: got new connection";
    QTcpSocket* socket = server->nextPendingConnection();
    new ActionReceiver(socket);
}

ActionServer::~ActionServer()
{
    server->close();
    delete server;
}
