/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <WorkQueue.hpp>

#include <QDebug>
#include <QElapsedTimer>
#include <QDataStream>

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
		items.append(item);
		queueEmpty = false;
		updateStatus();
		if(canWork) workCondition.wakeOne();
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
	WorkQueueItem item(actionGroup, action, data);
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
		emit simulating(isSimulating);
		workCondition.wakeOne();
	mutex->unlock();
}

WorkQueueItem* WorkQueue::popItem()
{
	mutex->lock();
		if(!((simulationSteps>0) || continuousSimulationRunning))
		{
			isSimulating = false;
			emit simulating(isSimulating);
		}
		if(!canWork)
		{
			workCondition.wait(mutex);
		}

		WorkQueueItem* item;
		if(items.count()>0)
		{
			item = new WorkQueueItem(items.takeFirst());
		}
		else
		{
			item = new WorkQueueItem(ActionGroups::workQueue, WorkQueueActions::calculateStep);
			if(simulationSteps>0)
			{
				simulationSteps--;
			}
			else if(!continuousSimulationRunning)
			{
				qDebug()<<"error!";
			}
		}
		if(item->actionGroup == ActionGroups::workQueue &&
			item->action == WorkQueueActions::calculateStep)
		{
			if(sendFramesRegularly && animationTimer->elapsed()>(1000/60))
			{
				animationTimer->restart();
				WorkQueueItem item2(ActionGroups::workQueue, WorkQueueActions::prepareFrameData);
				items.prepend(item2);
			}
		}
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
	WorkQueueItem item(ActionGroups::workQueue, WorkQueueActions::stopWorker);
	pushItem(item);
}

void WorkQueue::sendFrameData()
{
	if(!sendFramesRegularly) return;
	WorkQueueItem item(ActionGroups::workQueue, WorkQueueActions::prepareFrameData);
	pushItem(item);
}
