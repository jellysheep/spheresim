/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _WORKQUEUE_HPP_
#define _WORKQUEUE_HPP_

#include "Actions.hpp"

#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <list>
#include <string>

class QElapsedTimer;

namespace SphereSim
{

    /** \brief Info about a work to be done by the worker. */
    class WorkQueueItem
    {
    public:
        /** \see ActionGroups */
        unsigned char actionGroup;

        /** \see ActionGroups
         * \see WorkQueueAction */
        unsigned char action;

        /** \brief Any data or parameter for the work item. */
        std::string data;

        /** \brief Initialize WorkQueueItem. */
        WorkQueueItem(const unsigned char actGrp, const unsigned char act,
            const std::string d = std::string())
            :actionGroup(actGrp), action(act), data(d)
        {
        }

        WorkQueueItem() = delete;
        WorkQueueItem(const WorkQueueItem&) = default;
        WorkQueueItem& operator=(const WorkQueueItem&) = default;

    };

    /** \brief Storage for work to be done by the worker. */
    class WorkQueue : public QObject
    {
        Q_OBJECT

    private:
        /** \brief Items of the queue. */
        std::list<WorkQueueItem> items;

        /** \brief Mutex used to lock the work status. */
        QMutex* mutex;

        /** \brief Flag for work queue emptyness. */
        bool queueEmpty;

        /** \brief Flag showing if there is work to do. */
        bool canWork;

        /** \brief Wait condition controlling the worker. */
        QWaitCondition workCondition;

        /** \brief Number of steps to simulate. */
        unsigned int simulationSteps;

        /** \brief Flag showing if a continuous simulation is running. */
        bool continuousSimulationRunning;

        /** \brief Flag showing if the worker is simulating. */
        bool isSimulating;

        /** \brief Animation timer. */
        QElapsedTimer* animationTimer;

        /** \brief Animation timer. */
        const bool &sendFramesRegularly;

        /** \brief Update the canWork flag. */
        void updateStatus();

    public:
        /** \brief Initialize member variables. */
        WorkQueue(QMutex* mutex, const bool &frameSending);

        /** \brief Clean up member variables. */
        ~WorkQueue();

        WorkQueue() = delete;
        WorkQueue(const WorkQueue&) = delete;
        WorkQueue& operator=(const WorkQueue&) = delete;

        /** \brief Add a new item to the end of the work queue. */
        void pushItem(WorkQueueItem& item);

        /** \copybrief pushItem */
        void pushItem(unsigned char actionGroup, unsigned char action, std::string data);

        /** \brief Increase simulation steps (0 = start continuous simulation). */
        void pushSimulationSteps(unsigned int steps);

        /** \brief Return the next work to do and remove it from the queue. */
        WorkQueueItem* popItem();

        /** \brief Send updated frame data to client. */
        void sendFrameData();

    public slots:
        /** \brief Stop a running simulation. */
        void stopSimulation();

        /** \brief Stop the worker. */
        void stop();

    signals:
        void simulating(bool isSimulating);

    };

}

#endif
