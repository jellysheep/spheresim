/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
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
		void handleAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle BasicActions requests.
		 * \copydetails handleAction */
		void handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle SpheresUpdatingActions requests.
		 * \copydetails handleAction */
		void handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle CalculationActions requests.
		 * \copydetails handleAction */
		void handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle InformationActions requests.
		 * \copydetails handleAction */
		void handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle SimulatedSystemActions requests.
		 * \copydetails handleAction */
		void handleSimulatedSystemAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle WorkQueueActions requests.
		 * \copydetails handleAction */
		void handleWorkQueueAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle unknown action group requests.
		 * \copydetails handleAction */
		void handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle unknown action requests.
		 * \copydetails handleAction */
		void handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data);
		
	public:
		/** \brief Initialize member variables. */
		SimulationWorker(SphereCalculator* sphCalc, WorkQueue* queue, ActionReceiver* actRcv);
		
		/** \brief Clean up member variables. */
		~SimulationWorker();
		
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
