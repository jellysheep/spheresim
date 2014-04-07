/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "WorkQueue.hpp"
#include "SphereTransmit.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QDataStream>
#include <sstream>

using namespace SphereSim;

WorkQueue::WorkQueue(QMutex* mutex, const bool &frameSending)
    :items(), mutex(mutex), queueEmpty(true), canWork(false), workCondition(),
    simulationSteps(0), continuousSimulationRunning(false), isSimulating(false),
    animationTimer(new QElapsedTimer()), sendFramesRegularly(frameSending)
{
    updateStatus();
    animationTimer->start();
}

WorkQueue::~WorkQueue()
{
    delete mutex;
    delete animationTimer;
}

void WorkQueue::pushItem(WorkQueueItem& item)
{
    mutex->lock();
        items.push_back(item);
        queueEmpty = false;
        updateStatus();
        if (canWork)
        {
            workCondition.wakeOne();
        }
    mutex->unlock();
}

void WorkQueue::pushItem(unsigned char actionGroup, unsigned char action,
    std::string data)
{
    if (actionGroup == ActionGroups::calculation)
    {
        std::istringstream stream(data);
        unsigned int steps;
        bool actionDone = false;
        switch (action)
        {
        case CalculationActions::calculateStep:
            pushSimulationSteps(1);
            actionDone = true;
            break;
        case CalculationActions::calculateSomeSteps:
            steps = readInt(stream);
            pushSimulationSteps(steps);
            actionDone = true;
            break;
        case CalculationActions::startSimulation:
            pushSimulationSteps(0);
            actionDone = true;
            break;
        case CalculationActions::stopSimulation:
            stopSimulation();
            actionDone = true;
            break;
        }
        if (actionDone)
        {
            return;
        }
    }
    WorkQueueItem item(actionGroup, action, data);
    pushItem(item);
}

void WorkQueue::pushSimulationSteps(unsigned int steps)
{
    mutex->lock();
        if (steps == 0)
        {
            simulationSteps = 0;
            continuousSimulationRunning = true;
        }
        else
        {
            simulationSteps += steps;
        }
        updateStatus();
        isSimulating = true;
        emit simulating(isSimulating);
        workCondition.wakeOne();
    mutex->unlock();
}

WorkQueueItem* WorkQueue::popItem()
{
    mutex->lock();
        if (((simulationSteps>0) || continuousSimulationRunning) == false)
        {
            isSimulating = false;
            emit simulating(isSimulating);
        }
        if (canWork == false)
        {
            workCondition.wait(mutex);
        }

        WorkQueueItem* item;
        if (items.size()>0)
        {
            item = new WorkQueueItem(items.front());
            items.pop_front();
        }
        else
        {
            item = new WorkQueueItem(ActionGroups::workQueue,
                WorkQueueActions::calculateStep);
            if (simulationSteps>0)
            {
                simulationSteps--;
            }
            else if (continuousSimulationRunning == false)
            {
                qDebug()<<"error!";
            }
        }
        if (item->actionGroup == ActionGroups::workQueue &&
            item->action == WorkQueueActions::calculateStep)
        {
            if (sendFramesRegularly && animationTimer->elapsed()>(1000/60))
            {
                animationTimer->restart();
                WorkQueueItem item2(ActionGroups::workQueue,
                    WorkQueueActions::prepareFrameData);
                items.push_front(item2);
            }
        }
        if (items.size()<=0)
        {
            queueEmpty = true;
            updateStatus();
        }
    mutex->unlock();
    return item;
}

void WorkQueue::updateStatus()
{
    canWork = (queueEmpty == false) || (simulationSteps>0)
        || continuousSimulationRunning;
}

void WorkQueue::stopSimulation()
{
    mutex->lock();
        continuousSimulationRunning = false;
        simulationSteps = 0;
        updateStatus();
    mutex->unlock();
}

void WorkQueue::stop()
{
    WorkQueueItem item(ActionGroups::workQueue, WorkQueueActions::stopWorker);
    pushItem(item);
}

void WorkQueue::sendFrameData()
{
    if (sendFramesRegularly == false)
    {
        return;
    }
    WorkQueueItem item(ActionGroups::workQueue, WorkQueueActions::prepareFrameData);
    pushItem(item);
}
