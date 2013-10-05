/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SIMULATIONWORKER_HPP_
#define _SIMULATIONWORKER_HPP_

#include <QObject>

namespace SphereSim{
	
	class SphereCalculator;
	class WorkQueue;
	
	/**
	 * \brief Does calculation work.
	 */
	class SimulationWorker : public QObject{
		Q_OBJECT
		
	private:
		/** \brief Currently used SphereCalculator. */
		SphereCalculator* sphCalc;
		
		/** \brief Flag if worker is running (true) or supposed to stop (false). */
		bool running;
		
		/** \brief Queue storing the simulation work. */
		WorkQueue* queue;
		
	public:
		SimulationWorker(SphereCalculator* sphCalc, WorkQueue* queue);
		~SimulationWorker();
		
	public slots:
		/** \brief Starts to work. */
		void work();
		
		/** \brief Stops and deletes the worker. */
		void stop();
		
	signals:
		/** \brief Shows that the worker stopped working. */
		void finished();
		
	};
	
}

#endif
