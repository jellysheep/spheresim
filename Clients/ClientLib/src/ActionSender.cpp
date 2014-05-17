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

#include <QCoreApplication>
#include <QTimer>
#include <nanomsg/pubsub.h>
#include <chrono>
#include <random>
#include <string>

Q_DECLARE_METATYPE(std::string);

using namespace SphereSim;

ActionSender::ActionSender(const char* addr, unsigned short sendPort,
    unsigned short recvPort, QObject* client)
    :sendSocket(AF_SP, NN_PUB), recvSocket(AF_SP, NN_SUB),
    connectedFlag(false), clientID(0), frameBuffer(30),
    lastServerStatus(ServerStatusReplies::acknowledge),
    receivedServerReply(false), lastServerReplyData(), framerateTimer(),
    frameCounter(0), oldFrameCounter(0), receivedFramesPerSecond(0),
    messageTransmitter(new MessageTransmitter(&sendSocket, &recvSocket)),
    readyToRun(false), heartbeatTimer(),
    failureExitWhenDisconnected(false),  simulatedSystem(nullptr)
{
    qRegisterMetaType<std::string>();
    //~ connect(socket, SIGNAL(connected()), SLOT(connected()));
    //~ connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(messageTransmitter, SIGNAL(processData(std::string)),
        SLOT(processReply(std::string)));
    framerateTimer.start();
    frameBuffer.setActionSender(this);
    connect(this, SIGNAL(newFrameReceived()), SLOT(framerateEvent()));
    connect(this, SIGNAL(serverReady()), client, SLOT(run()),
        Qt::QueuedConnection);
    connect(&heartbeatTimer, SIGNAL(timeout()), SLOT(heartbeat()));

    std::ostringstream sendAddress;
    sendAddress<<"tcp://"<<addr<<':'<<sendPort;
    sendSocket.connect(sendAddress.str().c_str());
    std::ostringstream recvAddress;
    recvAddress<<"tcp://"<<addr<<':'<<recvPort;
    recvSocket.connect(recvAddress.str().c_str());
    recvSocket.setsockopt(NN_SUB, NN_SUB_SUBSCRIBE, "", 0);

    simulatedSystem = new SimulatedSystem();
    connect(simulatedSystem, SIGNAL(variableToSend(std::string)),
        SLOT(sendVariable(std::string)));
    connect(simulatedSystem, SIGNAL(variableUpdated(int)),
        SLOT(variableUpdated(int)));

    std::random_device randomDevice;
    std::uniform_int_distribution<unsigned int> distribution;
    clientID = distribution(randomDevice);
    Console()<<"ActionSender: ClientID: "<<std::hex<<clientID<<".\n";
    messageTransmitter->start();
    heartbeatTimer.start(1000);
}

ActionSender::~ActionSender()
{
    if (simulatedSystem != nullptr)
    {
        delete simulatedSystem;
    }
    delete messageTransmitter;
}

void ActionSender::sendAction(unsigned char actionGroup, unsigned char action,
    const std::string& arr)
{
    std::ostringstream data;
    writeInt(data, clientID);
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
    const unsigned int receiverClientID = readInt(stream);
    if (receiverClientID != clientID)
    {
        return;
    }
    lastServerStatus = readShort(stream);

    if (lastServerStatus == ServerStatusReplies::sendFrame)
    {
        if (readyToRun == false)
        {
            return;
        }
        unsigned int serverFrameCounter = readInt(stream);
        if (serverFrameCounter > frameCounter)
        {
            frameBuffer.pushFrame();
            frameCounter = serverFrameCounter;
            emit newFrameReceived();
        }
        unsigned short sphereIndex = readShort(stream);
        Sphere sphere;
        readBasicSphereData(stream, sphere);
        frameBuffer.pushElement(sphere);
    }
    else if (lastServerStatus == ServerStatusReplies::sendVariable)
    {
        unsigned short _var = readShort(stream);
        SimulationVariables::Variable var = (SimulationVariables::Variable)_var;
        std::string varData = data.substr(8);
        simulatedSystem->receiveVariable(var, varData);
    }
    else if (lastServerStatus == ServerStatusReplies::clientAccepted)
    {
        if (connectedFlag == true)
        {
            return;
        }
        Console()<<"ActionSender: connected to host.\n";
        connectedFlag = true;
        std::ostringstream stream;
        writeInt(stream, clientID);
        std::string idString = stream.str();
        recvSocket.setsockopt(NN_SUB, NN_SUB_SUBSCRIBE,
            idString.c_str(), idString.size());
        recvSocket.setsockopt(NN_SUB, NN_SUB_UNSUBSCRIBE, "", 0);
        simulatedSystem->sendAllVariables();
        readyToRun = true;
        emit serverReady();
    }
    else
    {
        std::string replyData = data.substr(6);
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
    return connectedFlag;
}

FrameBuffer<Sphere>* ActionSender::getFrameBuffer()
{
    return &frameBuffer;
}

void ActionSender::terminateServer()
{
    sendAction(ActionGroups::basic, BasicActions::terminateServer);
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
        receivedFramesPerSecond = (frameCounter-oldFrameCounter)
            *1000.0/framerateTimer.elapsed();
        oldFrameCounter = frameCounter;
        emit framerateUpdate();
        framerateTimer.restart();
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
        frameBuffer.updateElementsPerFrame(sphCount);
    }
}

void ActionSender::heartbeat()
{
    sendAction(ActionGroups::basic, BasicActions::heartbeat);
}
