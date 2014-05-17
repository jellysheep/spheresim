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

ActionReceiver::ActionReceiver(const unsigned int clientID)
    :clientID(clientID), simulatedSystem(), sphCalc(this, &simulatedSystem),
    workQueue(sphCalc.getWorkQueue()), clientAccepted(false)
{
    connect(&sphCalc, SIGNAL(frameToSend(std::string)), SLOT(sendFrame(std::string)));
    connect(&simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
    connect(workQueue, SIGNAL(simulating(bool)), SLOT(simulating(bool)));
}

ActionReceiver::~ActionReceiver()
{
    Console()<<"ActionReceiver: disconnected.\n";
}

void ActionReceiver::terminateServer()
{
#ifndef NDEBUG
    Console()<<"Server terminating...\n";
    emit qApp->quit();
    deleteLater();
#endif /*NDEBUG*/
}

void ActionReceiver::processRequest(std::string data)
{
    unsigned char actionGroup = data[0];
    unsigned char action = data[1];
    if (clientAccepted == false && actionGroup == ActionGroups::basic
        && action == BasicActions::heartbeat)
    {
        clientAccepted = true;
        sendReply(ServerStatusReplies::clientAccepted, std::string());
        return;
    }
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

void ActionReceiver::sendReply(unsigned short serverStatus, std::string dataToSend)
{
    std::ostringstream data;
    writeShort(data, serverStatus);
    data<<dataToSend;
    emit send(clientID, data.str());
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
