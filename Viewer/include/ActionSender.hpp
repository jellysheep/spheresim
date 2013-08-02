
#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_
/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>
#include <QAbstractSocket>
#include <QProcess>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Starts client and builds up connection to server;
	 * sends actions to server when requested.
	 */
	class ActionSender:QObject{
		Q_OBJECT
	private:
		/** \brief Address of the current connection. */
		QHostAddress* addr;
		
		/** \brief Port of the current connection. */
		quint16 port;
		
		/** \brief Socket of the current connection. */
		QTcpSocket* socket;
		
		/** \brief Stores if connected to server. */
		bool connectedFlag;
		
		/** \brief Tells how often connection creating was tried. */
		unsigned int connectionTryCount;
		
		/** \brief Process of the automatically started server. */
		QProcess process;
		
		/** \brief Tells if a server was automatically started. */
		bool createdOwnServer;
		
		/**
		 * \brief Sends an action request and data to the server.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 * \param data Data that will be sent with the action request.
		 */
		void sendAction(const unsigned char actionGroup, const unsigned char action, const QByteArray& data);
		/**
		 * \brief Sends an action request to the server.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 */
		void sendAction(const unsigned char actionGroup, const unsigned char action);
		
		/**
		 * \brief Sends an action request and data to the server and returns server reply.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 * \param data Data that will be sent with the action request.
		 * \return Reply data from the server.
		 */
		QByteArray sendReplyAction(const unsigned char actionGroup, const unsigned char action, const QByteArray& data);
		/**
		 * \brief Sends an action request to the server and returns server reply.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 * \return Reply data from the server.
		 */
		QByteArray sendReplyAction(const unsigned char actionGroup, const unsigned char action);
		
	public:
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(const QHostAddress& addr, const quint16 port);
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(const QString& addr, const quint16 port);
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(const char* addr, const quint16 port);
		
		~ActionSender();
		
		/**
		 * \brief Asks server for the version number and returns it.
		 * \return The version string reported by the server.
		 */
		const QString getVersion();
		
		/**
		 * \brief Asks server for a "true" string and returns it.
		 * \return The "true" string sent by the server.
		 */
		const QString getTrueString();
		
		/**
		 * \brief Shows if socket is connected.
		 * \return True if connected, false if not.
		 */
		bool isConnected();
		
		/**
		 * \brief Requests the server to add one sphere.
		 * \return Current sphere count reported by server.
		 */
		unsigned int addSphere();
		
		/**
		 * \brief Requests the server to remove the last sphere.
		 * \return Current sphere count reported by server.
		 */
		unsigned int removeLastSphere();
		
		/**
		 * \brief Requests the server to send the current sphere count.
		 * \return Current sphere count reported by server.
		 */
		unsigned int getSphereCount();
	
	public slots:
		void connected(){
			connectedFlag = true;
		}
	};
	
}

#endif
