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

#include <nanomsg/pair.h>
#include <sstream>

using namespace SphereSim;

ActionServer::ActionServer(const char* addr, unsigned short port)
    :socket(AF_SP, NN_PAIR), actionReceiver(nullptr)
{
    Console()<<"ActionServer: constructor called.\n";
    std::ostringstream address;
    address<<"tcp://"<<addr<<':'<<port;
    socket.bind(address.str().c_str());
    Console()<<"ActionServer: listening did succeed.\n";
    actionReceiver = new ActionReceiver(&socket);
}

ActionServer::~ActionServer()
{
    if (actionReceiver != nullptr)
    {
        delete actionReceiver;
    }
}
