/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Connection.hpp"
#include "MessageTransmitter.hpp"

#include <QTcpSocket>

using namespace SphereSim;

MessageTransmitter::MessageTransmitter(QTcpSocket* socket)
    :socket(socket), partialData(), collectingData(false)
{
}

std::string MessageTransmitter::encode(const std::string& data)
{
    std::string encodedData(2*data.size(), '\0');
    for(unsigned int i = 0; i<data.size(); i++)
    {
        encodedData[2*i] = 'a'+data[i]/16;
        encodedData[2*i+1] = 'a'+data[i]%16;
    }
    return encodedData;
}

std::string MessageTransmitter::decode(const std::string& data)
{
    std::string decodedData(data.size()/2, '\0');
    for(unsigned int i = 0; i<data.size()/2; i++)
    {
        decodedData[i] = ((data[2*i]-'a')*16)+(data[2*i+1]-'a');
    }
    return decodedData;
}

void MessageTransmitter::send(std::string data)
{
    std::ostringstream finalData;
    finalData<<Connection::startByte;
    finalData<<encode(data);
    finalData<<Connection::endByte;
    socket->write(finalData.str().c_str());
}

void MessageTransmitter::readData()
{
    QByteArray byteArray = socket->readAll();
    std::string data(byteArray.constData(), byteArray.size());

    std::string::size_type endIndex, startIndex;
    bool dataLeft;
    do
    {
        dataLeft = false;
        endIndex = data.find(Connection::endByte);
        startIndex = data.find(Connection::startByte);

        if (endIndex == std::string::npos)
        {
            if (startIndex == std::string::npos)
            {
                ///no endByte or startByte
                if (collectingData)
                {
                    partialData<<data;
                }
            }
            else
            {
                ///only startByte
                if (collectingData == false)
                {
                    //what if last reply did not end correctly? next reply would be
                    //skipped (waiting for endByte)...
                    collectingData = true;
                    partialData.str(data.substr(startIndex-1));
                }
            }
        }
        else
        {
            if (startIndex == std::string::npos)
            {
                ///only endByte
                if (collectingData)
                {
                    partialData<<data.substr(0, endIndex);
                    collectingData = false;
                    emit processData(decode(partialData.str()));
                }
            }
            else
            {
                ///startByte and endByte
                if (startIndex<endIndex)
                {
                    ///startByte before endByte
                    partialData.str(
                        data.substr(startIndex+1, endIndex-startIndex-1));
                    collectingData = false;
                    emit processData(decode(partialData.str()));
                    data = data.substr(endIndex+1);
                    dataLeft = true;
                }
                else
                {
                    ///endByte before startByte
                    if (collectingData)
                    {
                        partialData<<data.substr(0, endIndex);
                        collectingData = false;
                        emit processData(decode(partialData.str()));
                        data = data.substr(endIndex+1);
                        dataLeft = true;
                    }
                }
            }
        }
    }
    while(dataLeft);
}
