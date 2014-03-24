/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <SimulationWorker.hpp>
#include <SphereCalculator.hpp>
#include <WorkQueue.hpp>
#include <Console.hpp>
#include <ActionReceiver.hpp>
#include <Version.hpp>
#include <Connection.hpp>
#include <SphereTransmit.hpp>

#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>

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
    QByteArray data;
    SimulationVariables::Variable var;
    int _var;
    switch (workQueueItem->action)
    {
    case BasicActions::terminateServer:
        emit sendReply(ServerStatusReplies::terminating, "Server terminating...");
        actRcv->terminateServer();
        break;
    case BasicActions::updateVariable:
        _var = ((char)workQueueItem->data.at(0))*256
            + (char)workQueueItem->data.at(1);
        var = (SimulationVariables::Variable)_var;
        data = workQueueItem->data.mid(2);
        sphCalc->simulatedSystem->receiveVariable(var, data);
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleSpheresUpdatingAction(WorkQueueItem* workQueueItem)
{
    quint16 i;
    Sphere s;
    QDataStream stream(&workQueueItem->data, QIODevice::ReadOnly);
    QByteArray retData;
    QDataStream retStream(&retData, QIODevice::WriteOnly);
    Scalar s1, s2;
    switch (workQueueItem->action)
    {
    case SpheresUpdatingActions::addSphere:
        emit sendReply(ServerStatusReplies::acknowledge,
            QString::number(sphCalc->addSphere()).toUtf8());
        break;
    case SpheresUpdatingActions::removeLastSphere:
        emit sendReply(ServerStatusReplies::acknowledge,
            QString::number(sphCalc->removeLastSphere()).toUtf8());
        break;
    case SpheresUpdatingActions::updateSphere:
        stream>>i;
        readAllSphereData(stream, s);
        sphCalc->updateSphere(i, s);
        break;
    case SpheresUpdatingActions::getBasicSphereData:
        stream>>i;
        s = sphCalc->getAllSphereData(i);
        writeBasicSphereData(retStream, s);
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    case SpheresUpdatingActions::getAllSphereData:
        stream>>i;
        s = sphCalc->getAllSphereData(i);
        writeAllSphereData(retStream, s);
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    case SpheresUpdatingActions::addSomeSpheres:
        stream>>i;
        emit sendReply(ServerStatusReplies::acknowledge,
            QString::number(sphCalc->addSomeSpheres(i)).toUtf8());
        break;
    case SpheresUpdatingActions::removeSomeLastSpheres:
        stream>>i;
        emit sendReply(ServerStatusReplies::acknowledge,
            QString::number(sphCalc->removeSomeLastSpheres(i)).toUtf8());
        break;
    case SpheresUpdatingActions::updateSpherePositionsInBox:
        stream>>s1;
        stream>>s2;
        sphCalc->updateSpherePositionsInBox(s1, s2);
        break;
    case SpheresUpdatingActions::updateAllSpheres:
        readAllSphereData(stream, s);
        sphCalc->updateAllSpheres(s);
        break;
    case SpheresUpdatingActions::updateKineticEnergy:
        stream>>s1;
        sphCalc->updateKineticEnergy(s1);
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleCalculationAction(WorkQueueItem* workQueueItem)
{
    QDataStream stream(&workQueueItem->data, QIODevice::ReadOnly);
    QByteArray retData;
    QDataStream retStream(&retData, QIODevice::WriteOnly);
    switch (workQueueItem->action)
    {
    case CalculationActions::popCalculationCounter:
        retStream<<sphCalc->popCalculationCounter();
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    case CalculationActions::popStepCounter:
        retStream<<sphCalc->popStepCounter();
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    case CalculationActions::getLastStepCalculationTime:
        retStream<<sphCalc->getLastStepCalculationTime();
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    default:
        handleUnknownAction(workQueueItem);
        break;
    }
}

void SimulationWorker::handleInformationAction(WorkQueueItem* workQueueItem)
{
    QDataStream stream(&workQueueItem->data, QIODevice::ReadOnly);
    QByteArray retData;
    QDataStream retStream(&retData, QIODevice::WriteOnly);
    switch (workQueueItem->action)
    {
    case InformationActions::getTotalEnergy:
        retStream<<sphCalc->getTotalEnergy();
        emit sendReply(ServerStatusReplies::acknowledge, retData);
        break;
    case InformationActions::getKineticEnergy:
        retStream<<sphCalc->getKineticEnergy();
        emit sendReply(ServerStatusReplies::acknowledge, retData);
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
    qWarning()<<"SimulationWorker: Warning: received unknown action group"
        <<Connection::startByte<<(int)workQueueItem->actionGroup
            <<(int)workQueueItem->action<<Connection::endByte;
    emit sendReply(ServerStatusReplies::unknownActionGroup, "unknown action group");
}

void SimulationWorker::handleUnknownAction(WorkQueueItem* workQueueItem)
{
    qWarning()<<"SimulationWorker: Warning: received unknown action"
        <<Connection::startByte<<(int)workQueueItem->actionGroup
            <<(int)workQueueItem->action<<Connection::endByte;
    emit sendReply(ServerStatusReplies::unknownAction, "unknown action");
}
