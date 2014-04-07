/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionReceiver.hpp"
#include "Connection.hpp"
#include "WorkQueue.hpp"
#include "DataTransmit.hpp"
#include "MessageTransmitter.hpp"

#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* socket)
    :socket(socket), messageTransmitter(new MessageTransmitter(socket)),
    simulatedSystem(), sphCalc(this, &simulatedSystem),
    workQueue(sphCalc.getWorkQueue())
{
    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), messageTransmitter, SLOT(readData()));
    connect(messageTransmitter, SIGNAL(processData(std::string)),
        SLOT(processRequest(std::string)));
    connect(&sphCalc, SIGNAL(frameToSend(std::string)), SLOT(sendFrame(std::string)));
    connect(&simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
    connect(workQueue, SIGNAL(simulating(bool)), SLOT(simulating(bool)));
    simulatedSystem.sendAllVariables();
}

ActionReceiver::~ActionReceiver()
{
    delete messageTransmitter;
    if(socket != NULL)
    {
        socket->close();
        delete socket;
        socket = NULL;
    }
    qDebug()<<"ActionReceiver: disconnected";
}

void ActionReceiver::terminateServer()
{
    qDebug()<<"Server terminating...";
    if(socket != NULL)
    {
        socket->close();
        delete socket;
        socket = NULL;
    }
    emit QCoreApplication::instance()->quit();
    deleteLater();
}

void ActionReceiver::processRequest(std::string data)
{
    unsigned char actionGroup = data[0];
    unsigned char action = data[1];
    if (data.size()>2)
    {
        data = data.substr(2);
    }
    else
    {
        data.clear();
    }
    workQueue->pushItem(actionGroup, action, data);
}

void ActionReceiver::sendReply(unsigned char serverStatus, std::string dataToSend)
{
    std::ostringstream data;
    writeChar(data, serverStatus);
    data<<dataToSend;
    messageTransmitter->send(data.str());
}

void ActionReceiver::sendFrame(std::string frameToSend)
{
    sendReply(ServerStatusReplies::sendFrame, frameToSend);
}

void ActionReceiver::sendVariable(std::string variableToSend)
{
    sendReply(ServerStatusReplies::sendVariable, variableToSend);
}

void ActionReceiver::simulating(bool isSimulating)
{
    simulatedSystem.set(SimulationVariables::simulating, isSimulating);
}
