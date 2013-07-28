/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _ACTIONSERVER_HPP_
#define _ACTIONSERVER_HPP_

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>

class QTcpServer;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Starts server and waits for incoming connections from clients; 
	 * replies to client requests.
	 */
	class ActionServer:private QObject{
		Q_OBJECT
	private:
		/** \brief Holds the listening server object. */
		QTcpServer* server;
	public:
		/**
		 * \brief Starts a QTcpServer and listens to the specified port.
		 * \param addr The address that the server will be listening to. 
		 * \param port The port that the server will be listening to. 
		 */
		ActionServer(const QHostAddress& addr, const quint16 port);
		/**
		 * \brief Starts a QTcpServer and listens to the specified port.
		 * \param addr The address that the server will be listening to. 
		 * \param port The port that the server will be listening to. 
		 */
		ActionServer(const QString& addr, const quint16 port);
		/**
		 * \brief Starts a QTcpServer and listens to the specified port.
		 * \param addr The address that the server will be listening to. 
		 * \param port The port that the server will be listening to. 
		 */
		ActionServer(const char* addr, const quint16 port);
		
		~ActionServer();
		
	public slots:
		/**
		 * \brief Receives connection from client and creates a new ActionReceiver for it. 
		 */
		void newConnection();
	};
	
}

#endif
