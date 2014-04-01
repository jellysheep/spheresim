/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionSender.hpp>
#include <Connection.hpp>
#include <Console.hpp>
#include <SphereTransmit.hpp>

#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QCoreApplication>

using namespace SphereSim;

ActionSender::ActionSender(QStringList args, const char* addr, quint16 port,
    QObject* client)
    :socket(new QTcpSocket()),
    connectedFlag(false), connectionTryCount(0), serverProcess(),
    createdOwnServer(false), frameBuffer(10),
    lastServerStatus(ServerStatusReplies::acknowledge),
    receivedServerReply(false), lastServerReplyData(), replyData(),
    collectingReplyData(false), framerateTimer(), frameCounter(0),
    receivedFramesPerSecond(0), failureExitWhenDisconnected(false),
    simulatedSystem(nullptr)
{
    connect(socket, SIGNAL(connected()), SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), SLOT(readData()));
    framerateTimer.start();
    frameBuffer.setActionSender(this);
    connect(this, SIGNAL(newFrameReceived()), SLOT(framerateEvent()));
    while (connectionTryCount<1000 && (connectedFlag == false))
    {
        socket->connectToHost(QHostAddress(addr), port);
        connectionTryCount++;
        socket->waitForConnected(100);
        if (connectedFlag == false)
        {
            qDebug()<<"ActionSender: retrying to connect to host.";
            if (connectionTryCount<=1)
            {
                qDebug()<<"ActionSender: starting Server.";
                serverProcess.start("SphereSim_Server");
                createdOwnServer = true;
            }
        }
    }
    simulatedSystem = new SimulatedSystem();
    connect(simulatedSystem, SIGNAL(variableToSend(QByteArray)),
        SLOT(sendVariable(QByteArray)));
    connect(simulatedSystem, SIGNAL(variableUpdated(int)),
        SLOT(variableUpdated(int)));
    connect(simulatedSystem, SIGNAL(receivedAllVariables()), client, SLOT(run()),
        Qt::QueuedConnection);
    simulatedSystem->sendAllVariables();
}

ActionSender::~ActionSender()
{
    if (createdOwnServer)
    {
        sendReplyAction(ActionGroups::basic, BasicActions::terminateServer);
        serverProcess.waitForFinished(200);
        qDebug()<<"ActionSender: killing Server.";
        serverProcess.terminate();
        serverProcess.waitForFinished(200);
        serverProcess.kill();
    }
    socket->close();
    delete socket;
}

void ActionSender::sendAction(quint8 actionGroup, quint8 action, QByteArray& arr)
{
    QByteArray data;
    data.append(actionGroup);
    data.append(action);
    data.append(arr);
    data = data.toBase64();
    data.prepend(Connection::startByte);
    data.append(Connection::endByte);
    socket->write(data);
}
void ActionSender::sendAction(quint8 actionGroup, quint8 action)
{
    QByteArray arr;
    sendAction(actionGroup, action, arr);
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action,
    QByteArray& arr)
{
    sendAction(actionGroup, action, arr);
    while (receivedServerReply == false)
    {
        QCoreApplication::processEvents();
    }
    receivedServerReply = false;
    return lastServerReplyData;
}

QByteArray ActionSender::sendReplyAction(quint8 actionGroup, quint8 action)
{
    QByteArray arr;
    return sendReplyAction(actionGroup, action, arr);
}

void ActionSender::readData()
{
    QByteArray arr = socket->readAll();
    processData(arr);
}

void ActionSender::processData(QByteArray byteArray)
{
    qint16 endIndex, startIndex;
    endIndex = byteArray.indexOf(Connection::endByte);
    startIndex = byteArray.indexOf(Connection::startByte);

    if (endIndex<0)
    {
        if (startIndex<0)
        {
            ///no endByte or startByte
            if (collectingReplyData)
            {
                replyData.append(byteArray);
            }
        }
        else
        {
            ///only startByte
            if (collectingReplyData == false)
            {
                //what if last reply did not end correctly? next reply would be
                //skipped (waiting for endByte)...
                collectingReplyData = true;
                replyData = byteArray.right(byteArray.size()-startIndex-1);
            }
        }
    }
    else
    {
        if (startIndex<0)
        {
            ///only endByte
            if (collectingReplyData)
            {
                replyData.append(byteArray.left(endIndex));
                collectingReplyData = false;
                processReply();
            }
        }
        else
        {
            ///startByte and endByte
            if (startIndex<endIndex)
            {
                ///startByte before endByte
                replyData = byteArray.mid(startIndex+1, endIndex-startIndex-1);
                collectingReplyData = false;
                processReply();
                processData(byteArray.right(byteArray.size()-endIndex-1));
            }
            else
            {
                ///endByte before startByte
                if (collectingReplyData)
                {
                    replyData.append(byteArray.left(endIndex));
                    collectingReplyData = false;
                    processReply();
                    processData(byteArray.right(byteArray.size()-endIndex-1));
                }
            }
        }
    }
}

void ActionSender::processReply()
{
    QByteArray data = QByteArray::fromBase64(replyData);
    lastServerStatus = data[0];
    data = data.mid(1);

    if (lastServerStatus == ServerStatusReplies::sendFrame)
    {
        QDataStream stream(&data, QIODevice::ReadOnly);
        quint16 sphCount;
        stream>>sphCount;
        emit sphereCountChanged(sphCount);
        emit sphereCountChangedDouble((double)sphCount);
        frameBuffer.updateElementsPerFrame(sphCount);
        quint16 sphereIndex;
        Sphere sphere;
        while (stream.atEnd() == false)
        {
            stream>>sphereIndex;
            readBasicSphereData(stream, sphere);
            frameBuffer.pushElement(sphere);
        }
        frameBuffer.pushFrame();
        frameCounter++;
        emit newFrameReceived();
    }
    else if (lastServerStatus == ServerStatusReplies::sendVariable)
    {
        int _var = ((char)data.at(0))*256 + (char)data.at(1);
        SimulationVariables::Variable var = (SimulationVariables::Variable)_var;
        data = data.mid(2);
        simulatedSystem->receiveVariable(var, data);
    }
    else
    {
        lastServerReplyData = data;
        receivedServerReply = true;
    }
}

void ActionSender::updateSphereCount(quint16 sphereCount)
{
    frameBuffer.updateElementsPerFrame(sphereCount);
}

void ActionSender::willBeSimulating()
{
    simulatedSystem->set(SimulationVariables::simulating, true);
}

bool ActionSender::isConnected()
{
    socket->waitForConnected(10);
    return connectedFlag;
}

FrameBuffer<Sphere>* ActionSender::getFrameBuffer()
{
    return &frameBuffer;
}

quint16 ActionSender::addSphere()
{
    quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::addSphere)).toUInt();
    updateSphereCount(sphereCount);
    return sphereCount;
}

quint16 ActionSender::removeLastSphere()
{
    quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::removeLastSphere)).toUInt();
    updateSphereCount(sphereCount);
    return sphereCount;
}

void ActionSender::updateSphere(quint16 i, Sphere s)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<i;
    writeAllSphereData(stream, s);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateSphere, arr);
}

void ActionSender::getBasicSphereData(quint16 i, Sphere& s)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<i;
    QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::getBasicSphereData, arr);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    readBasicSphereData(retStream, s);
}

void ActionSender::getAllSphereData(quint16 i, Sphere& s)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<i;
    QByteArray retArr = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::getAllSphereData, arr);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    readAllSphereData(retStream, s);
}

quint16 ActionSender::addSomeSpheres(quint16 sphCount)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<sphCount;
    quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::addSomeSpheres, arr)).toUInt();
    updateSphereCount(sphereCount);
    return sphereCount;
}

quint16 ActionSender::removeSomeLastSpheres(quint16 sphCount)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<sphCount;
    quint16 sphereCount = QString(sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::removeSomeLastSpheres, arr)).toUInt();
    updateSphereCount(sphereCount);
    return sphereCount;
}

void ActionSender::updateSpherePositionsInBox(Scalar randomDisplacement,
    Scalar randomSpeed)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<randomDisplacement;
    stream<<randomSpeed;
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateSpherePositionsInBox, arr);
}

void ActionSender::updateAllSpheres(Sphere s)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    writeAllSphereData(stream, s);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateAllSpheres, arr);
}

void ActionSender::updateKineticEnergy(Scalar factor)
{
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<factor;
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateKineticEnergy, arr);
}

void ActionSender::calculateStep()
{
    sendAction(ActionGroups::calculation, CalculationActions::calculateStep);
    willBeSimulating();
}

quint32 ActionSender::popCalculationCounter()
{
    QByteArray retArr = sendReplyAction(ActionGroups::calculation,
        CalculationActions::popCalculationCounter);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    quint32 calculationCounter;
    retStream>>calculationCounter;
    return calculationCounter;
}

void ActionSender::calculateSomeSteps(quint32 steps)
{
    if (steps <= 0)
    {
        return;
    }
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream<<steps;
    sendAction(ActionGroups::calculation,
        CalculationActions::calculateSomeSteps, arr);
    willBeSimulating();
}

void ActionSender::startSimulation()
{
    sendAction(ActionGroups::calculation, CalculationActions::startSimulation);
    willBeSimulating();
}

void ActionSender::stopSimulation()
{
    sendAction(ActionGroups::calculation, CalculationActions::stopSimulation);
}

quint32 ActionSender::popStepCounter()
{
    QByteArray retArr = sendReplyAction(ActionGroups::calculation,
        CalculationActions::popStepCounter);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    quint32 stepCounter;
    retStream>>stepCounter;
    return stepCounter;
}

quint32 ActionSender::getLastStepCalculationTime()
{
    QByteArray retArr = sendReplyAction(ActionGroups::calculation,
        CalculationActions::getLastStepCalculationTime);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    quint32 lastStepCalculationTime;
    retStream>>lastStepCalculationTime;
    return lastStepCalculationTime;
}

Scalar ActionSender::getTotalEnergy()
{
    QByteArray retArr = sendReplyAction(ActionGroups::information,
        InformationActions::getTotalEnergy);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    Scalar totalEnergy;
    retStream>>totalEnergy;
    return totalEnergy;
}

Scalar ActionSender::getKineticEnergy()
{
    QByteArray retArr = sendReplyAction(ActionGroups::information,
        InformationActions::getKineticEnergy);
    QDataStream retStream(&retArr, QIODevice::ReadOnly);
    Scalar kineticEnergy;
    retStream>>kineticEnergy;
    return kineticEnergy;
}

void ActionSender::framerateEvent()
{
    if (framerateTimer.elapsed()>1000)
    {
        if (frameCounter > 0)
        {
            receivedFramesPerSecond = frameCounter*1000.0/framerateTimer.elapsed();
            frameCounter = 0;
            emit framerateUpdate();
        }
        framerateTimer.restart();
    }
}

void ActionSender::disconnected()
{
    qDebug()<<"Server connection closed."<<failureExitWhenDisconnected;
    emit connectionClosed();
    if (failureExitWhenDisconnected)
    {
        qApp->exit(1);
    }
}

void ActionSender::sendVariable(QByteArray variableToSend)
{
    sendAction(ActionGroups::basic, BasicActions::updateVariable, variableToSend);
}

void ActionSender::variableUpdated(int var)
{
    if (var == SimulationVariables::sphereCount)
    {
        quint16 sphCount =
            simulatedSystem->get<int>(SimulationVariables::sphereCount);
        emit sphereCountChanged(sphCount);
        emit sphereCountChangedDouble((double)sphCount);
    }
}
