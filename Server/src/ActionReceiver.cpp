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
#include "SphereTransmit.hpp"

#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* socket)
    :socket(socket), requestData(), collectingRequestData(false), simulatedSystem(),
    sphCalc(this, &simulatedSystem), workQueue(sphCalc.getWorkQueue())
{
    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(readData()));
    connect(&sphCalc, SIGNAL(frameToSend(std::string)), SLOT(sendFrame(std::string)));
    connect(&simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
    connect(workQueue, SIGNAL(simulating(bool)), SLOT(simulating(bool)));
    simulatedSystem.sendAllVariables();
}

ActionReceiver::~ActionReceiver()
{
    if(socket != NULL)
    {
        socket->close();
        delete socket;
        socket = NULL;
    }
    qDebug()<<"ActionReceiver: disconnected";
    delete socket;
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

void ActionReceiver::readData()
{
    std::string arr(socket->readAll().constData());
    processData(arr);
}

void ActionReceiver::processData(std::string byteArray)
{
    std::string::size_type endIndex, startIndex;
    endIndex = byteArray.find(Connection::endByte);
    startIndex = byteArray.find(Connection::startByte);

    if (endIndex == std::string::npos)
    {
        if (startIndex == std::string::npos)
        {
            ///no endByte or startByte
            if (collectingRequestData)
            {
                requestData.append(byteArray);
            }
        }
        else
        {
            ///only startByte
            if (collectingRequestData == false)
            {
                //what if last request did not end correctly? next request
                //would be skipped (waiting for endByte)...
                collectingRequestData = true;
                requestData = byteArray.substr(startIndex-1);
            }
        }
    }
    else
    {
        if (startIndex == std::string::npos)
        {
            ///only endByte
            if (collectingRequestData)
            {
                requestData.append(byteArray.substr(0, endIndex));
                collectingRequestData = false;
                processRequest();
            }
        }
        else
        {
            ///startByte and endByte
            if (startIndex<endIndex)
            {
                ///startByte before endByte
                requestData = byteArray.substr(startIndex+1, endIndex-startIndex-1);
                collectingRequestData = false;
                processRequest();
                processData(byteArray.substr(endIndex+1));
            }
            else
            {
                ///endByte before startByte
                if (collectingRequestData)
                {
                    requestData.append(byteArray.substr(0, endIndex));
                    collectingRequestData = false;
                    processRequest();
                    processData(byteArray.substr(endIndex+1));
                }
            }
        }
    }
}

void ActionReceiver::processRequest()
{
    QByteArray byteArray(requestData.c_str(), requestData.size());
    byteArray = QByteArray::fromBase64(byteArray);
    std::string data(byteArray.constData(), byteArray.size());
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
    std::ostringstream finalData;
    finalData<<Connection::startByte;
    std::string dataStr = data.str();
    QByteArray byteArray(dataStr.c_str(), dataStr.size());
    byteArray = byteArray.toBase64();
    dataStr = std::string(byteArray.constData(), byteArray.size());
    finalData<<dataStr;
    finalData<<Connection::endByte;
    socket->write(finalData.str().c_str());
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
