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
#include "DataTransmit.hpp"
#include "MessageTransmitter.hpp"

#include <QCoreApplication>
#include <nanomsg/pubsub.h>
#include <sstream>

using namespace SphereSim;

ActionServer::ActionServer(const char* addr,
    unsigned short sendPort, unsigned short recvPort)
    :sendSocket(AF_SP, NN_PUB), recvSocket(AF_SP, NN_SUB),
    messageTransmitter(nullptr), actionReceivers(), disconnectionTimer(this)
{
    Console()<<"ActionServer: constructor called.\n";
    std::ostringstream sendAddress;
    sendAddress<<"tcp://"<<addr<<':'<<sendPort;
    sendSocket.bind(sendAddress.str().c_str());
    std::ostringstream recvAddress;
    recvAddress<<"tcp://"<<addr<<':'<<recvPort;
    recvSocket.bind(recvAddress.str().c_str());
    recvSocket.setsockopt(NN_SUB, NN_SUB_SUBSCRIBE, "", 0);
    Console()<<"ActionServer: listening did succeed.\n";
    messageTransmitter = new MessageTransmitter(&sendSocket, &recvSocket);
    connect(messageTransmitter, SIGNAL(processData(std::string)),
        SLOT(receiveRequest(std::string)));
    connect(&disconnectionTimer, SIGNAL(timeout()), SLOT(disconnectionCheck()));
    disconnectionTimer.start(5000);
    messageTransmitter->start();
}

ActionServer::~ActionServer()
{
    tearDown();
}

void ActionServer::tearDown()
{
    for (ActionReceiverMap::iterator it = actionReceivers.begin();
        it != actionReceivers.end(); ++it)
    {
        delete &(*it->second);
    }
    actionReceivers.clear();
    if (messageTransmitter != nullptr)
    {
        messageTransmitter->deleteLater();
        messageTransmitter = nullptr;
    }
    qApp->quit();
}

void ActionServer::receiveRequest(std::string request)
{
    std::istringstream stream(request);
    unsigned int clientID = readInt(stream);
    ActionReceiver* actionReceiver;
    ActionReceiverMap::iterator it = actionReceivers.find(clientID);
    if (it != actionReceivers.end())
    {
        actionReceiver = it->second;
    }
    else
    {
        Console()<<"Registering ClientID "<<std::hex<<clientID<<".\n";
        actionReceiver = new ActionReceiver(clientID);
        connect(actionReceiver, SIGNAL(send(unsigned int, std::string)),
            SLOT(send(unsigned int, std::string)));
#ifndef NDEBUG
        connect(actionReceiver, SIGNAL(terminateServer()),
            SLOT(tearDown()));
#endif /*NDEBUG*/
        actionReceivers.emplace(clientID, actionReceiver);
    }
    request = request.substr(4);
    actionReceiver->processRequest(request);
}

void ActionServer::send(unsigned int clientID, std::string reply)
{
    if (actionReceivers.count(clientID) == 0)
    {
        Console()<<Console::red<<Console::bold<<"Bad server reply (ClientID).\n";
        return;
    }
    std::ostringstream stream;
    writeInt(stream, clientID);
    stream<<reply;
    messageTransmitter->send(stream.str());
}

void ActionServer::disconnectionCheck()
{
    for (ActionReceiverMap::iterator it = actionReceivers.begin();
        it != actionReceivers.end(); )
    {
        if (it->second->hasReceivedRequests() == false)
        {
            Console()<<"Deregistering ClientID "<<std::hex<<it->first<<".\n";
            delete &(*it->second);
            it = actionReceivers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
