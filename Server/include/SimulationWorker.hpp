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
	
	/**
	 * \brief Calculates sphere physics.
	 */
	class SimulationWorker : public QObject{
		Q_OBJECT
		
	private:
		SphereCalculator* sphCalc;
		bool running;
		
	public:
		SimulationWorker(SphereCalculator* sphCalc);
		~SimulationWorker();
		
	public slots:
		void work();
		void stop();
		
	signals:
		void finished();
		
	};
	
}

#endif
