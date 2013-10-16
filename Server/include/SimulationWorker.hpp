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
	
	/** \brief Worker for calculation work. */
	class SimulationWorker : public QObject
	{
		Q_OBJECT
		
	private:
		/** \brief Currently used SphereCalculator. */
		SphereCalculator* sphCalc;
		
		/** \brief Running flag for worker. */
		bool running;
		
		/** \brief Queue storing the simulation work. */
		WorkQueue* queue;
		
	public:
		/** \brief Initialize member variables. */
		SimulationWorker(SphereCalculator* sphCalc, WorkQueue* queue);
		
		/** \brief Clean up member variables. */
		~SimulationWorker();
		
	public slots:
		/** \brief Start working. */
		void work();
		
		/** \brief Stop and delete the worker. */
		void stop();
		
	signals:
		/** \brief Worker stopped working. */
		void finished();
		
	};
	
}

#endif
