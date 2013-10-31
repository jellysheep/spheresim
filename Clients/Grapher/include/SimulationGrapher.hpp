/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SIMULATIONGRAPHER_HPP_
#define _SIMULATIONGRAPHER_HPP_

#include <Sphere.hpp>

#include <QtGlobal>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QList>

class ActionSender;
class QHostAddress;
class QTimer;

namespace SphereSim
{
	
	/** \brief Grapher for server simulations. */
	class SimulationGrapher : public QObject
	{
		Q_OBJECT
		
	private:
		/** \brief Client object to communicate with server. */
		ActionSender* actionSender;
		
		QTimer* dataUpdateTimer;
		
		quint32 counter;
		
		Scalar timeStep;
		
		Scalar time;
		
		QString filename;
		
		QString filename2;
		
		quint16 sphereCountSqrt;
		
		quint16 sphereCount;
		
		quint16 dataPoints;
		
		QList<Scalar> data;
		
		QList<Scalar> data2;
		
		quint16 stepsToEquilibrium;
		
	public:
		/** \brief Start a SimulationGrapher with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to. */
		SimulationGrapher(QStringList args, QHostAddress addr, quint16 port);
		
		~SimulationGrapher();
		
	public slots:
		/** \brief Run the simulation and output results. */
		void runSimulation();
		
		void timerUpdate();
		
	};
	
}

#endif
