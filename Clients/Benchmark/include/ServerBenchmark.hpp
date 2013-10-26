/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SERVERBENCHMARK_HPP_
#define _SERVERBENCHMARK_HPP_

#include <Actions.hpp>
#include <Console.hpp>
#include <Version.hpp>

#include <QtGlobal>
#include <QObject>

class ActionSender;
class QHostAddress;

namespace SphereSim
{
	
	/** \brief Benchmark for server simulations. */
	class ServerBenchmark : public QObject
	{
		Q_OBJECT
		
	private:
		/** \brief Client object to communicate with server. */
		ActionSender* sender;
		
		/** \copydoc runBenchmark
		 * \param detectCollisions Flag for collision detection.
		 * \param calculateGravity Flag for gravity calculation. */
		void runBenchmark_internal(bool detectCollisions, bool calculateGravity);
		
	public:
		/** \brief Start a ServerBenchmark with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to. */
		ServerBenchmark(QStringList args, QHostAddress addr, quint16 port);
		
		~ServerBenchmark();
		
	public slots:
		/** \brief Run all benchmarks and output results. */
		void runBenchmark();
		
	};
	
}

#endif
