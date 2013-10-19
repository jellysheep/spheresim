/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _WORKQUEUE_HPP_
#define _WORKQUEUE_HPP_

#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QObject>

class QElapsedTimer;

namespace SphereSim
{
	
	/** \brief Type of work to be done by the worker. */
	namespace WorkQueueItemType
	{
		/** \copydoc WorkQueueItemType */
		enum ItemType{
			calculateStep,
			addSphere,
			removeSphere,
			updateSphere,
			stop,
			prepareFrameData
		};
	}
	
	/** \brief Info about a work to be done by the worker. */
	class WorkQueueItem{
	public:
		/** \copydoc WorkQueueItemType */
		WorkQueueItemType::ItemType type;
		/** \brief Any parameter for the work item. */
		void* param;
		
		/** \brief Initialize WorkQueueItem. */
		WorkQueueItem()
		{
			type = WorkQueueItemType::calculateStep;
			param = NULL;
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
		
	public:
		/** \brief Initialize member variables. */
		WorkQueue(QMutex* mutex);
		
		/** \brief Clean up member variables. */
		~WorkQueue();
		
		/** \brief Add a new item to the end of the work queue. */
		void pushItem(WorkQueueItem item);
		
		/** \brief Increase the number of steps to simulate (0 = start continuous simulation). */
		void pushSimulationSteps(quint32 steps);
		
		/** \brief Return the next work to do and remove it from the queue. */
		WorkQueueItem popItem();
		
		/** \copydoc isSimulating */
		bool getIsSimulating();
		
	public slots:
		/** \brief Stop a running simulation. */
		void stopSimulation();
		
		/** \brief Stop the worker. */
		void stop();
		
	};
	
}

#endif
