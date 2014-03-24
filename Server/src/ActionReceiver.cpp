/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionReceiver.hpp>
#include <Connection.hpp>
#include <WorkQueue.hpp>

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
    connect(&sphCalc, SIGNAL(frameToSend(QByteArray)), SLOT(sendFrame(QByteArray)));
    connect(&simulatedSystem, SIGNAL(variableToSend(QByteArray)),
        SLOT(sendVariable(QByteArray)));
    connect(workQueue, SIGNAL(simulating(bool)), SLOT(simulating(bool)));
    simulatedSystem.sendAllVariables();
}

ActionReceiver::~ActionReceiver()
{
    socket->close();
    qDebug()<<"ActionReceiver: disconnected";
    delete socket;
}

void ActionReceiver::terminateServer()
{
    qDebug()<<"Server terminating...";
    socket->close();
    delete socket;
    emit QCoreApplication::instance()->quit();
    deleteLater();
}

void ActionReceiver::readData()
{
    QByteArray arr = socket->readAll();
    processData(arr);
}

void ActionReceiver::processData(QByteArray byteArray)
{
    qint16 endIndex, startIndex;
    endIndex = byteArray.indexOf(Connection::endByte);
    startIndex = byteArray.indexOf(Connection::startByte);

    if (endIndex<0)
    {
        if (startIndex<0)
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
                requestData = byteArray.right(byteArray.size()-startIndex-1);
            }
        }
    }
    else
    {
        if (startIndex<0)
        {
            ///only endByte
            if (collectingRequestData)
            {
                requestData.append(byteArray.left(endIndex));
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
                requestData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
                collectingRequestData = false;
                processRequest();
                processData(byteArray.right(byteArray.size()-endIndex-1));
            }
            else
            {
                ///endByte before startByte
                if (collectingRequestData)
                {
                    requestData.append(byteArray.left(endIndex));
                    collectingRequestData = false;
                    processRequest();
                    processData(byteArray.right(byteArray.size()-endIndex-1));
                }
            }
        }
    }
}

void ActionReceiver::processRequest()
{
    QByteArray data = QByteArray::fromBase64(requestData);
    quint8 actionGroup = data[0];
    quint8 action = data[1];
    if (data.size()>2)
    {
        data = data.mid(2);
    }
    else
    {
        data.clear();
    }
    workQueue->pushItem(actionGroup, action, data);
}

void ActionReceiver::sendReply(quint8 serverStatus, QByteArray dataToSend)
{
    QByteArray data = dataToSend;
    data.prepend(serverStatus);
    data = data.toBase64();
    data.prepend(Connection::startByte);
    data.append(Connection::endByte);
    socket->write(data);
}

void ActionReceiver::sendFrame(QByteArray frameToSend)
{
    sendReply(ServerStatusReplies::sendFrame, frameToSend);
}

void ActionReceiver::sendVariable(QByteArray variableToSend)
{
    sendReply(ServerStatusReplies::sendVariable, variableToSend);
}

void ActionReceiver::simulating(bool isSimulating)
{
    simulatedSystem.set(SimulationVariables::simulating, isSimulating);
}
