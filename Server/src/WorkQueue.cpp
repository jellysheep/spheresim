/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <WorkQueue.hpp>

#include <QDebug>
#include <QElapsedTimer>
#include <QDataStream>

using namespace SphereSim;

WorkQueue::WorkQueue(QMutex* mutex_):items()
{
	mutex = mutex_;
	queueEmpty = true;
	simulationSteps = 0;
	continuousSimulationRunning = false;
	updateStatus();
	isSimulating = false;
	sendFramesRegularly = false;
	animationTimer = new QElapsedTimer();
	animationTimer->start();
}

WorkQueue::~WorkQueue()
{
	delete mutex;
	delete animationTimer;
}

void WorkQueue::pushItem(WorkQueueItem item)
{
	mutex->lock();
		items.append(item);
		queueEmpty = false;
		updateStatus();
		if(canWork) workCondition.wakeAll();
	mutex->unlock();
}

void WorkQueue::pushItem(quint8 actionGroup, quint8 action, QByteArray data)
{
	if(actionGroup == ActionGroups::calculation)
	{
		QDataStream stream(&data, QIODevice::ReadOnly);
		quint32 steps;
		bool actionDone = false;
		switch(action)
		{
		case CalculationActions::calculateStep:
			pushSimulationSteps(1);
			actionDone = true;
			break;
		case CalculationActions::calculateSomeSteps:
			stream>>steps;
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
		if(actionDone)
			return;
	}
	WorkQueueItem item;
	item.actionGroup = actionGroup;
	item.action = action;
	item.data = data;
	pushItem(item);
}

void WorkQueue::pushSimulationSteps(quint32 steps)
{
	mutex->lock();
		if(steps == 0)
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
		workCondition.wakeAll();
	mutex->unlock();
}

WorkQueueItem WorkQueue::popItem()
{
	mutex->lock();
		if(!((simulationSteps>0) || continuousSimulationRunning))
		{
			isSimulating = false;
		}
		if(!canWork)
		{
			workCondition.wait(mutex);
		}
	mutex->unlock();
	WorkQueueItem item = WorkQueueItem();
	if(items.count()>0)
	{
		item = items.takeFirst();
	}
	else
	{
		item = WorkQueueItem();
		if(simulationSteps>0)
		{
			simulationSteps--;
			item.actionGroup = ActionGroups::workQueue;
			item.action = WorkQueueActions::calculateStep;
		}
		else if(continuousSimulationRunning)
		{
			item.actionGroup = ActionGroups::workQueue;
			item.action = WorkQueueActions::calculateStep;
		}
		else
		{
			qDebug()<<"error!";
			item.actionGroup = ActionGroups::workQueue;
			item.action = WorkQueueActions::calculateStep;
		}
	}
	if(item.actionGroup == ActionGroups::workQueue &&
		item.action == WorkQueueActions::calculateStep)
	{
		if(sendFramesRegularly && animationTimer->elapsed()>(1000/60))
		{
			animationTimer->restart();
			WorkQueueItem item2 = WorkQueueItem();
			item2.actionGroup = ActionGroups::workQueue;
			item2.action = WorkQueueActions::prepareFrameData;
			items.prepend(item2);
		}
	}
	mutex->lock();
		if(items.count()<=0)
		{
			queueEmpty = true;
			updateStatus();
		}
	mutex->unlock();
	return item;
}

void WorkQueue::updateStatus()
{
	canWork = (!queueEmpty) || (simulationSteps>0) || continuousSimulationRunning;
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
	WorkQueueItem item;
	item.actionGroup = ActionGroups::workQueue;
	item.action = WorkQueueActions::stopWorker;
	pushItem(item);
}

bool WorkQueue::getIsSimulating()
{
	return isSimulating;
}

void WorkQueue::sendFrameData()
{
	if(!sendFramesRegularly) return;
	WorkQueueItem item;
	item.actionGroup = ActionGroups::workQueue;
	item.action = WorkQueueActions::prepareFrameData;
	pushItem(item);
}

void WorkQueue::updateFrameSending(bool sendFrames)
{
	sendFramesRegularly = sendFrames;
}
