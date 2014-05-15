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
#include "MessageTransmitter.hpp"

#include <nanomsg/pubsub.h>
#include <sstream>

using namespace SphereSim;

ActionServer::ActionServer(const char* addr,
    unsigned short sendPort, unsigned short recvPort)
    :sendSocket(AF_SP, NN_PUB), recvSocket(AF_SP, NN_SUB),
    actionReceiver(nullptr), messageTransmitter(nullptr)
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
    actionReceiver = new ActionReceiver();
    QObject::connect(messageTransmitter, SIGNAL(processData(std::string)),
        actionReceiver, SLOT(processRequest(std::string)));
    QObject::connect(actionReceiver, SIGNAL(sendReply(std::string)),
        messageTransmitter, SLOT(send(std::string)));
    messageTransmitter->start();
}

ActionServer::~ActionServer()
{
    if (actionReceiver != nullptr)
    {
        delete actionReceiver;
    }
    if (messageTransmitter != nullptr)
    {
        delete messageTransmitter;
    }
}
