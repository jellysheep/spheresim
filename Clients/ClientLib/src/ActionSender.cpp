/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionSender.hpp"
#include "Connection.hpp"
#include "Console.hpp"
#include "DataTransmit.hpp"
#include "MessageTransmitter.hpp"

#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QCoreApplication>
#include <string>

Q_DECLARE_METATYPE(std::string);

using namespace SphereSim;

ActionSender::ActionSender(const char* addr, unsigned short port,
    QObject* client)
    :socket(new QTcpSocket()),
    connectedFlag(false), connectionTryCount(0), serverProcess(),
    createdOwnServer(false), frameBuffer(10),
    lastServerStatus(ServerStatusReplies::acknowledge),
    receivedServerReply(false), lastServerReplyData(), framerateTimer(),
    frameCounter(0), receivedFramesPerSecond(0),
    messageTransmitter(new MessageTransmitter(socket)),
    failureExitWhenDisconnected(false), simulatedSystem(nullptr)
{
    qRegisterMetaType<std::string>();
    connect(socket, SIGNAL(connected()), SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), messageTransmitter, SLOT(readData()));
    connect(messageTransmitter, SIGNAL(processData(std::string)),
        SLOT(processReply(std::string)));
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
    connect(simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
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
    delete messageTransmitter;
    socket->close();
    delete socket;
}

void ActionSender::sendAction(unsigned char actionGroup, unsigned char action,
    const std::string& arr)
{
    std::ostringstream data;
    writeChar(data, actionGroup);
    writeChar(data, action);
    data<<arr;
    messageTransmitter->send(data.str());
}
void ActionSender::sendAction(unsigned char actionGroup, unsigned char action)
{
    sendAction(actionGroup, action, std::string());
}

std::string ActionSender::sendReplyAction(unsigned char actionGroup,
    unsigned char action, const std::string& arr)
{
    sendAction(actionGroup, action, arr);
    while (receivedServerReply == false)
    {
        QCoreApplication::processEvents();
    }
    receivedServerReply = false;
    return lastServerReplyData;
}

std::string ActionSender::sendReplyAction(unsigned char actionGroup,
    unsigned char action)
{
    return sendReplyAction(actionGroup, action, std::string());
}

void ActionSender::processReply(std::string data)
{
    std::istringstream stream(data);
    lastServerStatus = readChar(stream);

    if (lastServerStatus == ServerStatusReplies::sendFrame)
    {
        unsigned short sphCount = readShort(stream);
        emit sphereCountChanged(sphCount);
        emit sphereCountChangedDouble((double)sphCount);
        frameBuffer.updateElementsPerFrame(sphCount);
        Sphere sphere;
        while (stream.good())
        {
            readBasicSphereData(stream, sphere);
            frameBuffer.pushElement(sphere);
        }
        frameBuffer.pushFrame();
        frameCounter++;
        emit newFrameReceived();
    }
    else if (lastServerStatus == ServerStatusReplies::sendVariable)
    {
        unsigned short _var = readShort(stream);
        SimulationVariables::Variable var = (SimulationVariables::Variable)_var;
        std::string varData = data.substr(3);
        simulatedSystem->receiveVariable(var, varData);
    }
    else
    {
        std::string replyData = data.substr(1);
        lastServerReplyData = replyData;
        receivedServerReply = true;
    }
}

void ActionSender::updateSphereCount(unsigned short sphereCount)
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

unsigned short ActionSender::addSphere()
{
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::addSphere);
    std::istringstream retStream(retData);
    unsigned short sphereCount = readShort(retStream);
    updateSphereCount(sphereCount);
    return sphereCount;
}

unsigned short ActionSender::removeLastSphere()
{
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::removeLastSphere);
    std::istringstream retStream(retData);
    unsigned short sphereCount = readShort(retStream);
    updateSphereCount(sphereCount);
    return sphereCount;
}

void ActionSender::updateSphere(unsigned short i, Sphere s)
{
    std::ostringstream stream;
    writeShort(stream, i);
    writeAllSphereData(stream, s);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateSphere, stream.str());
}

void ActionSender::getBasicSphereData(unsigned short i, Sphere& s)
{
    std::ostringstream stream;
    writeShort(stream, i);
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::getBasicSphereData, stream.str());
    std::istringstream retStream(retData);
    readBasicSphereData(retStream, s);
}

void ActionSender::getAllSphereData(unsigned short i, Sphere& s)
{
    std::ostringstream stream;
    writeShort(stream, i);
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::getAllSphereData, stream.str());
    std::istringstream retStream(retData);
    readAllSphereData(retStream, s);
}

unsigned short ActionSender::addSomeSpheres(unsigned short sphCount)
{
    std::ostringstream stream;
    writeShort(stream, sphCount);
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::addSomeSpheres, stream.str());
    std::istringstream retStream(retData);
    unsigned short sphereCount = readShort(retStream);
    updateSphereCount(sphereCount);
    return sphereCount;
}

unsigned short ActionSender::removeSomeLastSpheres(unsigned short sphCount)
{
    std::ostringstream stream;
    writeShort(stream, sphCount);
    std::string retData = sendReplyAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::removeSomeLastSpheres, stream.str());
    std::istringstream retStream(retData);
    unsigned short sphereCount = readShort(retStream);
    updateSphereCount(sphereCount);
    return sphereCount;
}

void ActionSender::updateSpherePositionsInBox(Scalar randomDisplacement,
    Scalar randomSpeed)
{
    std::ostringstream stream;
    writeDouble(stream, randomDisplacement);
    writeDouble(stream, randomSpeed);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateSpherePositionsInBox, stream.str());
}

void ActionSender::updateAllSpheres(Sphere s)
{
    std::ostringstream stream;
    writeAllSphereData(stream, s);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateAllSpheres, stream.str());
}

void ActionSender::updateKineticEnergy(Scalar factor)
{
    std::ostringstream stream;
    writeDouble(stream, factor);
    sendAction(ActionGroups::spheresUpdating,
        SpheresUpdatingActions::updateKineticEnergy, stream.str());
}

void ActionSender::calculateStep()
{
    sendAction(ActionGroups::calculation, CalculationActions::calculateStep);
    willBeSimulating();
}

unsigned int ActionSender::popCalculationCounter()
{
    std::string retData = sendReplyAction(ActionGroups::calculation,
        CalculationActions::popCalculationCounter);
    std::istringstream retStream(retData);
    unsigned int calculationCounter = readInt(retStream);
    return calculationCounter;
}

void ActionSender::calculateSomeSteps(unsigned int steps)
{
    if (steps <= 0)
    {
        return;
    }
    std::ostringstream stream;
    writeInt(stream, steps);
    sendAction(ActionGroups::calculation,
        CalculationActions::calculateSomeSteps, stream.str());
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

unsigned int ActionSender::popStepCounter()
{
    std::string retData = sendReplyAction(ActionGroups::calculation,
        CalculationActions::popStepCounter);
    std::istringstream retStream(retData);
    unsigned int stepCounter = readInt(retStream);
    return stepCounter;
}

unsigned int ActionSender::getLastStepCalculationTime()
{
    std::string retData = sendReplyAction(ActionGroups::calculation,
        CalculationActions::getLastStepCalculationTime);
    std::istringstream retStream(retData);
    unsigned int lastStepCalculationTime = readInt(retStream);
    return lastStepCalculationTime;
}

Scalar ActionSender::getTotalEnergy()
{
    std::string retData = sendReplyAction(ActionGroups::information,
        InformationActions::getTotalEnergy);
    std::istringstream retStream(retData);
    Scalar totalEnergy = readDouble(retStream);
    return totalEnergy;
}

Scalar ActionSender::getKineticEnergy()
{
    std::string retData = sendReplyAction(ActionGroups::information,
        InformationActions::getKineticEnergy);
    std::istringstream retStream(retData);
    Scalar kineticEnergy = readDouble(retStream);
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

void ActionSender::sendVariable(std::string variableToSend)
{
    sendAction(ActionGroups::basic, BasicActions::updateVariable, variableToSend);
}

void ActionSender::variableUpdated(int var)
{
    if (var == SimulationVariables::sphereCount)
    {
        unsigned short sphCount =
            simulatedSystem->get<unsigned int>(SimulationVariables::sphereCount);
        emit sphereCountChanged(sphCount);
        emit sphereCountChangedDouble((double)sphCount);
    }
}
