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

MessageTransmitter::MessageTransmitter(nn::socket* socket)
    :socket(socket), timer(this)
{
    timer.setInterval(10);
    connect(&timer, SIGNAL(timeout()), this, SLOT(readData()));
}

std::string MessageTransmitter::encode(const std::string& data)
{
    std::string encodedData(2*data.size(), '\0');
    for (unsigned int i = 0; i<data.size(); i++)
    {
        encodedData[2*i] = 'a'+data[i]/16;
        encodedData[2*i+1] = 'a'+data[i]%16;
    }
    return encodedData;
}

std::string MessageTransmitter::decode(const std::string& data)
{
    std::string decodedData(data.size()/2, '\0');
    for (unsigned int i = 0; i<data.size()/2; i++)
    {
        decodedData[i] = ((data[2*i]-'a')*16)+(data[2*i+1]-'a');
    }
    return decodedData;
}

void MessageTransmitter::start()
{
    timer.start();
}

void MessageTransmitter::send(std::string data)
{
    data = encode(data);
    //~ Console()<<"send: "<<data<<".\n";
    if (data.size() > 184)
    {
        abort();
    }
    std::ostringstream finalData;
    finalData<<Connection::startByte;
    finalData<<data;
    finalData<<Connection::endByte;
    std::string str = finalData.str();
    socket->send(str.c_str(), str.size(), NN_DONTWAIT);
}

void MessageTransmitter::readData()
{
    constexpr int buffer_length = 1024;
    while (true)
    {
        char buffer[buffer_length];
        int length = socket->recv(buffer, buffer_length, NN_DONTWAIT);
        if (length <= 0)
        {
            break;
        }
        std::string data(buffer+1, length-2);
        //~ Console()<<"recv: "<<data<<".\n";
        emit processData(decode(data));
    }
}
