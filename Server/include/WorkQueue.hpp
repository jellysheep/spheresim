/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _WORKQUEUE_HPP_
#define _WORKQUEUE_HPP_

#include <Actions.hpp>

#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QObject>

class QElapsedTimer;

namespace SphereSim
{
	
	/** \brief Info about a work to be done by the worker. */
	class WorkQueueItem{
	public:
		/** \see ActionGroups */
		quint8 actionGroup;
		
		/** \see ActionGroups
		 * \see WorkQueueAction */
		quint8 action;
		
		/** \brief Any data or parameter for the work item. */
		QByteArray data;
		
		/** \brief Initialize WorkQueueItem. */
		WorkQueueItem():data()
		{
			actionGroup = ActionGroups::calculation;
			action = CalculationActions::calculateStep;
		}
	};
	
	/** \brief Storage for work to be done by the worker. */
	class WorkQueue : public QObject{
		Q_OBJECT
		
	private:
		/** \brief Items of the queue. */
		QList<WorkQueueItem> items;
		
		/** \brief Mutex used to lock the work status. */
		QMutex* mutex;
		
		/** \brief Flag for work queue emptyness. */
		bool queueEmpty;
		
		/** \brief Flag showing if there is work to do. */
		bool canWork;
		
		/** \brief Wait condition controlling the worker. */
		QWaitCondition workCondition;
		
		/** \brief Number of steps to simulate. */
		quint32 simulationSteps;
		
		/** \brief Flag showing if a continuous simulation is running. */
		bool continuousSimulationRunning;
		
		/** \brief Flag showing if the worker is simulating. */
		bool isSimulating;
		
		/** \brief Update the canWork flag. */
		void updateStatus();
		
		/** \brief Animation timer. */
		QElapsedTimer* animationTimer;
		
		/** \brief Animation timer. */
		bool sendFramesRegularly;
		
	public:
		/** \brief Initialize member variables. */
		WorkQueue(QMutex* mutex);
		
		/** \brief Clean up member variables. */
		~WorkQueue();
		
		/** \brief Add a new item to the end of the work queue. */
		void pushItem(WorkQueueItem item);
		
		/** \copybrief pushItem */
		void pushItem(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Increase the number of steps to simulate (0 = start continuous simulation). */
		void pushSimulationSteps(quint32 steps);
		
		/** \brief Return the next work to do and remove it from the queue. */
		WorkQueueItem popItem();
		
		/** \copydoc isSimulating */
		bool getIsSimulating();
		
		/** \brief Send updated frame data to client. */
		void sendFrameData();
		
		/** \copydoc CalculationActions::updateFrameSending */
		void updateFrameSending(bool sendFramesRegularly);
		
	public slots:
		/** \brief Stop a running simulation. */
		void stopSimulation();
		
		/** \brief Stop the worker. */
		void stop();
		
	};
	
}

#endif
