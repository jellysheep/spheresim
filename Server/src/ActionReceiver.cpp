/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionReceiver.hpp"
#include "Connection.hpp"
#include "Console.hpp"
#include "WorkQueue.hpp"
#include "DataTransmit.hpp"
#include "MessageTransmitter.hpp"

#include <QCoreApplication>

using namespace SphereSim;

ActionReceiver::ActionReceiver(nn::socket* sendSocket, nn::socket* recvSocket)
    :sendSocket(sendSocket), recvSocket(recvSocket),
    messageTransmitter(new MessageTransmitter(sendSocket, recvSocket)),
    simulatedSystem(), sphCalc(this, &simulatedSystem),
    workQueue(sphCalc.getWorkQueue())
{
    connect(messageTransmitter, SIGNAL(processData(std::string)),
        SLOT(processRequest(std::string)));
    connect(messageTransmitter, SIGNAL(receiveTimeout()), &sphCalc,
        SLOT(resetServer()));
    connect(&sphCalc, SIGNAL(frameToSend(std::string)), SLOT(sendFrame(std::string)));
    connect(&simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
    connect(&simulatedSystem, SIGNAL(serverReady()), SLOT(serverReady()));
    connect(workQueue, SIGNAL(simulating(bool)), SLOT(simulating(bool)));
    messageTransmitter->start();
}

ActionReceiver::~ActionReceiver()
{
    delete messageTransmitter;
    Console()<<"ActionReceiver: disconnected.\n";
}

void ActionReceiver::terminateServer()
{
    Console()<<"Server terminating...\n";
    emit qApp->quit();
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

void ActionReceiver::serverReady()
{
    sendReply(ServerStatusReplies::serverReady, "");
}
