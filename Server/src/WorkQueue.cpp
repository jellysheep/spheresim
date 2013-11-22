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
			item.type = WorkQueueItemType::calculateStep;
		}
		else if(continuousSimulationRunning)
		{
			item.type = WorkQueueItemType::calculateStep;
		}
		else
		{
			qDebug()<<"error!";
			item.type = WorkQueueItemType::calculateStep;
		}
	}
	if(item.type == WorkQueueItemType::calculateStep)
	{
		if(sendFramesRegularly && animationTimer->elapsed()>(1000/60))
		{
			animationTimer->restart();
			WorkQueueItem item2 = WorkQueueItem();
			item2.type = WorkQueueItemType::prepareFrameData;
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
	item.type = WorkQueueItemType::stop;
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
	item.type = WorkQueueItemType::prepareFrameData;
	pushItem(item);
}

void WorkQueue::updateFrameSending(bool sendFrames)
{
	sendFramesRegularly = sendFrames;
}
