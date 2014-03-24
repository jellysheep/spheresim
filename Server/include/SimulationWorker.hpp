/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SIMULATIONWORKER_HPP_
#define _SIMULATIONWORKER_HPP_

#include <QObject>

namespace SphereSim
{

	class SphereCalculator;
	class WorkQueue;
	class WorkQueueItem;
	class ActionReceiver;

	/** \brief Worker for calculation work. */
	class SimulationWorker : public QObject
	{
		Q_OBJECT

	private:
		/** \brief Currently used SphereCalculator. */
		SphereCalculator* sphCalc;

		/** \brief Currently used ActionReceiver. */
		ActionReceiver* actRcv;

		/** \brief Running flag for worker. */
		bool running;

		/** \brief Queue storing the simulation work. */
		WorkQueue* queue;

		bool hasFinished;

		/** \brief Handle any action request and forward to specific handlers.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request. */
		void handleAction(WorkQueueItem* workQueueItem);

		/** \brief Handle BasicActions requests.
		 * \copydetails handleAction */
		void handleBasicAction(WorkQueueItem* workQueueItem);

		/** \brief Handle SpheresUpdatingActions requests.
		 * \copydetails handleAction */
		void handleSpheresUpdatingAction(WorkQueueItem* workQueueItem);

		/** \brief Handle CalculationActions requests.
		 * \copydetails handleAction */
		void handleCalculationAction(WorkQueueItem* workQueueItem);

		/** \brief Handle InformationActions requests.
		 * \copydetails handleAction */
		void handleInformationAction(WorkQueueItem* workQueueItem);

		/** \brief Handle SimulatedSystemActions requests.
		 * \copydetails handleAction */
		void handleSimulatedSystemAction(WorkQueueItem* workQueueItem);

		/** \brief Handle WorkQueueActions requests.
		 * \copydetails handleAction */
		void handleWorkQueueAction(WorkQueueItem* workQueueItem);

		/** \brief Handle unknown action group requests.
		 * \copydetails handleAction */
		void handleUnknownActionGroup(WorkQueueItem* workQueueItem);

		/** \brief Handle unknown action requests.
		 * \copydetails handleAction */
		void handleUnknownAction(WorkQueueItem* workQueueItem);

	public:
		/** \brief Initialize member variables. */
		SimulationWorker(SphereCalculator* sphCalc, WorkQueue* queue,
			ActionReceiver* actRcv);

		/** \brief Clean up member variables. */
		~SimulationWorker();

		SimulationWorker() = delete;
		SimulationWorker(const SimulationWorker&) = delete;
		SimulationWorker& operator=(const SimulationWorker&) = delete;

		bool getHasFinished();

	public slots:
		/** \brief Start working. */
		void work();

		/** \brief Stop and delete the worker. */
		void stop();

	signals:
		/** \brief Worker stopped working. */
		void finished();

		void sendReply(quint8 serverStatus, QByteArray data);

	};

}

#endif
