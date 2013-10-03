/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <SimulationWorker.hpp>
#include <SphereCalculator.hpp>
#include <Console.hpp>

#include <QDebug>

using namespace SphereSim;

SimulationWorker::SimulationWorker(SphereCalculator* sphCalc_, quint32 steps_){
	sphCalc = sphCalc_;
	steps = steps_;
	running = true;
}

void SimulationWorker::work(){
	quint32 counter = 0;
	while(running && (steps == 0 || (counter++)<steps)){
		sphCalc->integrateRungeKuttaStep();
	}
	sphCalc->simulationRunning = false;
	emit finished();
}

void SimulationWorker::stop(){
	running = false;
}
