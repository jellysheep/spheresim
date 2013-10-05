/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SERVERBENCHMARK_HPP_
#define _SERVERBENCHMARK_HPP_

#include <Actions.hpp>
#include <Console.hpp>
#include <Version.hpp>

#include <QtGlobal>
#include <QObject>

class ActionSender;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Benchmarks server calculation actions.
	 */
	class ServerBenchmark : public QObject{
		Q_OBJECT
		
	private:
		/** \brief Holds the client object to communicate with server. */
		ActionSender* sender;
		
	public:
		/**
		 * \brief Starts a ServerBenchmark with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerBenchmark(QStringList args, QHostAddress addr, quint16 port);
		/**
		 * \brief Starts a ServerBenchmark with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerBenchmark(QStringList args, QString addr, quint16 port);
		/**
		 * \brief Starts a ServerBenchmark with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerBenchmark(QStringList args, const char* addr, quint16 port);
		
		~ServerBenchmark();
		
	public slots:
		/**
		 * \brief Runs all benchmarks and outputs results.
		 */
		void runBenchmark();
		
	};
	
}

#endif
