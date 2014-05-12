/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "SimulationWorker.hpp"
#include "SphereCalculator.hpp"
#include "WorkQueue.hpp"
#include "ActionReceiver.hpp"
#include "Version.hpp"
#include "Connection.hpp"
#include "DataTransmit.hpp"

#include <QCoreApplication>
#include <sstream>

using namespace SphereSim;

SimulationWorker::SimulationWorker(SphereCalculator* sphCalc, WorkQueue* queue,
    ActionReceiver* actRcv)
    :sphCalc(sphCalc), actRcv(actRcv), running(true), queue(queue),
    hasFinished(false)
{
}

SimulationWorker::~SimulationWorker()
{
    delete queue;
}

void SimulationWorker::work()
{
    WorkQueueItem* workQueueItem;
    while (running)
    {
        workQueueItem = queue->popItem();
        handleAction(workQueueItem);
        delete workQueueItem;
        workQueueItem = nullptr;
        QCoreApplication::processEvents();
    }
    hasFinished = true;
    emit finished();
}

void SimulationWorker::stop()
{
    running = false;
}

bool SimulationWorker::getHasFinished()
{
    return hasFinished;
}

void SimulationWorker::handleAction(WorkQueueItem* workQueueItem)
{
    switch (workQueueItem->actionGroup)
    {
    case ActionGroups::basic:
        handleBasicAction(workQueueItem);
        break;
    case ActionGroups::spheresUpdating:
        handleSpheresUpdatingAction(workQueueItem);
        break;
    case ActionGroups::calculation:
        handleCalculationAction(workQueueItem);
        break;
    case ActionGroups::information:
        handleInformationAction(workQueueItem);
        break;
    case ActionGroups::workQueue:
        handleWorkQueueAction(workQueueItem);
        break;
    default:
        handleUnknownActionGroup(workQueueItem);
        break;
    }
}

void SimulationWorker::handleBasicAction(WorkQueueItem* workQueueItem)
{
    std::istringstream stream(workQueueItem->data);
    std::string varData;
    SimulationVariables::Variable var;
    unsigned short _var;
    switch (workQueueItem->action)
    {
    case BasicActions::terminateServer:
        emit sendReply(ServerStatusReplies::terminating, "Server terminating...");
        actRcv->terminateServer();
        break;
    case BasicActions::updateVariable:
        _var = readShort(stream);
        var = (SimulationVariables::Variable)_var;
        varData = workQueueItem->data.substr(2);
        sphCalc->simulatedSystem->receiveVariable(var, varData);
        break;
    case BasicActions::heartbeat:
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleSpheresUpdatingAction(WorkQueueItem* workQueueItem)
{
    unsigned short i;
    Sphere s;
    std::istringstream stream(workQueueItem->data);
    std::ostringstream retStream;
    Scalar s1, s2;
    switch (workQueueItem->action)
    {
    case SpheresUpdatingActions::addSphere:
        writeShort(retStream, sphCalc->addSphere());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::removeLastSphere:
        writeShort(retStream, sphCalc->removeLastSphere());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::updateSphere:
        i = readShort(stream);
        readAllSphereData(stream, s);
        sphCalc->updateSphere(i, s);
        break;
    case SpheresUpdatingActions::getBasicSphereData:
        i = readShort(stream);
        s = sphCalc->getAllSphereData(i);
        writeBasicSphereData(retStream, s);
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::getAllSphereData:
        i = readShort(stream);
        s = sphCalc->getAllSphereData(i);
        writeAllSphereData(retStream, s);
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::addSomeSpheres:
        i = readShort(stream);
        writeShort(retStream, sphCalc->addSomeSpheres(i));
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::removeSomeLastSpheres:
        i = readShort(stream);
        writeShort(retStream, sphCalc->removeSomeLastSpheres(i));
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case SpheresUpdatingActions::updateSpherePositionsInBox:
        s1 = readDouble(stream);
        s2 = readDouble(stream);
        sphCalc->updateSpherePositionsInBox(s1, s2);
        break;
    case SpheresUpdatingActions::updateAllSpheres:
        readAllSphereData(stream, s);
        sphCalc->updateAllSpheres(s);
        break;
    case SpheresUpdatingActions::updateKineticEnergy:
        s1 = readDouble(stream);
        sphCalc->updateKineticEnergy(s1);
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleCalculationAction(WorkQueueItem* workQueueItem)
{
    std::istringstream stream(workQueueItem->data);
    std::ostringstream retStream;
    switch (workQueueItem->action)
    {
    case CalculationActions::popCalculationCounter:
        writeInt(retStream, sphCalc->popCalculationCounter());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case CalculationActions::popStepCounter:
        writeInt(retStream, sphCalc->popStepCounter());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case CalculationActions::getLastStepCalculationTime:
        writeInt(retStream, sphCalc->getLastStepCalculationTime());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleInformationAction(WorkQueueItem* workQueueItem)
{
    std::istringstream stream(workQueueItem->data);
    std::ostringstream retStream;
    switch (workQueueItem->action)
    {
    case InformationActions::getTotalEnergy:
        writeDouble(retStream, sphCalc->getTotalEnergy());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    case InformationActions::getKineticEnergy:
        writeDouble(retStream, sphCalc->getKineticEnergy());
        emit sendReply(ServerStatusReplies::acknowledge, retStream.str());
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleWorkQueueAction(WorkQueueItem* workQueueItem)
{
    switch (workQueueItem->action)
    {
    case WorkQueueActions::stopWorker:
        stop();
        break;
    case WorkQueueActions::prepareFrameData:
        sphCalc->prepareFrameData();
        break;
    case WorkQueueActions::calculateStep:
        sphCalc->integrateRungeKuttaStep();
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleUnknownActionGroup(WorkQueueItem* workQueueItem)
{
    Console()<<"SimulationWorker: Warning: received unknown action group "
        <<(int)workQueueItem->actionGroup<<'|'<<(int)workQueueItem->action<<".\n";
    emit sendReply(ServerStatusReplies::unknownActionGroup, "unknown action group");
}

void SimulationWorker::handleUnknownAction(WorkQueueItem* workQueueItem)
{
    Console()<<"SimulationWorker: Warning: received unknown action "
        <<(int)workQueueItem->actionGroup<<'|'<<(int)workQueueItem->action<<".\n";
    emit sendReply(ServerStatusReplies::unknownAction, "unknown action");
}
