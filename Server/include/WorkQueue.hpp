/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _WORKQUEUE_HPP_
#define _WORKQUEUE_HPP_

#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QObject>

namespace SphereSim{
	
	/**
	 * \brief Types of work to be done by the worker.
	 */
	namespace WorkQueueItemType{
		enum ItemType{
			calculateStep,
			addSphere,
			removeSphere,
			updateSphere,
			stop
		};
	}
	
	/**
	 * \brief Info about a work to be done by the worker.
	 */
	class WorkQueueItem{
	public:
		WorkQueueItemType::ItemType type;
		void* param;
	};
	
	/**
	 * \brief Stores work to be done by the worker.
	 */
	class WorkQueue : public QObject{
		Q_OBJECT
		
	private:
		/** \brief Stores the items of the queue. */
		QList<WorkQueueItem> items;
		
		/** \brief Mutex used to lock the work status. */
		QMutex* mutex;
		
		/** \brief Flag showing if the work queue is empty. */
		bool queueEmpty;
		
		/** \brief Flag showing if there is work to do. */
		bool canWork;
		
		/** \brief Wait condition controlling the worker. */
		QWaitCondition workCondition;
		
		/** \brief Number of steps to simulate. */
		quint32 simulationSteps;
		
		/** \brief Flag showing if a continuous simulation is running. */
		bool continuousSimulationRunning;
		
		/** \brief Flag showing if the worker still has to simulate. */
		bool isSimulating;
		
		/** \brief Updates the canWork flag. */
		void updateStatus();
		
	public:
		WorkQueue(QMutex* mutex);
		~WorkQueue();
		
		/** \brief Adds a new item to the end of the work queue. */
		void pushItem(WorkQueueItem item);
		
		/** \brief Increases the number of steps to simulate (0 = start continuous simulation). */
		void pushSimulationSteps(quint32 steps);
		
		/** \brief Return the next work to do and remove it from the queue. */
		WorkQueueItem popItem();
		
		/** \brief Shows if the worker still has to simulate. */
		bool getIsSimulating();
		
	public slots:
		/** \brief Stops a running simulation. */
		void stopSimulation();
		
		/** \brief Stops the worker. */
		void stop();
		
	};
	
}

#endif
