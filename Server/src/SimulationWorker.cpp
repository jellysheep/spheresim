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

SimulationWorker::SimulationWorker(SphereCalculator* sphCalc_){
	sphCalc = sphCalc_;
	running = true;
}

SimulationWorker::~SimulationWorker(){
}

void SimulationWorker::work(){
	sphCalc->simulationRunning = true;
	quint32 i = 0;
	while(running){
		sphCalc->doStep();
	}
	sphCalc->simulationRunning = false;
	emit finished();
}

void SimulationWorker::stop(){
	running = false;
}
