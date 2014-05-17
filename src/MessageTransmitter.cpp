/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Connection.hpp"
#include "Console.hpp"
#include "MessageTransmitter.hpp"

using namespace SphereSim;

MessageTransmitter::MessageTransmitter(nn::socket* sendSocket,
    nn::socket* recvSocket)
    :sendSocket(sendSocket), recvSocket(recvSocket),
    timer(this), elapsedTimer(), connected(false)
{
    timer.setInterval(10);
    connect(&timer, SIGNAL(timeout()), this, SLOT(readData()));
}

void MessageTransmitter::start()
{
    timer.start();
    elapsedTimer.start();
}

void MessageTransmitter::send(std::string data)
{
    //~ Console()<<"send: "<<data<<".\n";
    sendSocket->send(data.c_str(), data.size(), NN_DONTWAIT);
}

void MessageTransmitter::readData()
{
    constexpr int bufferLength = 1024;
    while (true)
    {
        char buffer[bufferLength];
        int length = recvSocket->recv(buffer, bufferLength, NN_DONTWAIT);
        if (length <= 0)
        {
            if (connected && nn_errno() == EAGAIN && elapsedTimer.elapsed()>2000)
            {
                connected = false;
                emit receiveTimeout();
            }
            break;
        }
        connected = true;
        elapsedTimer.restart();
        std::string data(buffer, length);
        //~ Console()<<"recv: "<<data<<".\n";
        emit processData(data);
    }
}
