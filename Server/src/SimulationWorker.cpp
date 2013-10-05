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
#include <WorkQueue.hpp>
#include <Console.hpp>

#include <QDebug>
#include <QCoreApplication>

using namespace SphereSim;

SimulationWorker::SimulationWorker(SphereCalculator* sphCalc_, WorkQueue* queue_){
	sphCalc = sphCalc_;
	running = true;
	queue = queue_;
}

SimulationWorker::~SimulationWorker(){
	delete queue;
}

void SimulationWorker::work(){
	WorkQueueItem workQueueItem;
	while(running){
		workQueueItem = queue->popItem();
		if(workQueueItem.type == WorkQueueItemType::calculateStep){
			sphCalc->integrateRungeKuttaStep();
		}else if(workQueueItem.type == WorkQueueItemType::stop){
			running = false;
		}
		QCoreApplication::processEvents();
	}
	emit finished();
}

void SimulationWorker::stop(){
	running = false;
}
