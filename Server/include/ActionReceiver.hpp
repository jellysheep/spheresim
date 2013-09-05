/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _ACTIONRECEIVER_HPP_
#define _ACTIONRECEIVER_HPP_

#include <Actions.hpp>
#include <SphereManager.hpp>
#include <SphereCalculator.hpp>

#include <QObject>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Takes client socket and replies to client requests.
	 */
	class ActionReceiver:public QObject{
		Q_OBJECT
		
	private:
		/** \brief Holds the socket of the connection. */
		QTcpSocket* socket;
		
		/** \brief Collected data from a client request. */
		QByteArray requestData;
		
		/** \brief Flag if currently data from a client request is being collected;
		 * if true, no new requests are accepted. */
		bool collectingRequestData;
		
		/**
		 * \brief Processes received request data.
		 * \param byteArray Data from network stream to process.
		 */
		void processData(QByteArray byteArray);
		
		/** \brief Processes and answers received request. */
		void processRequest();
		
		/**
		 * \brief Handles any action request.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles basic action requests.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles requests that updates spheres.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles requests that calculate spheres physics.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles requests that informs about sphere properties.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles all unknown action group requests.
		 * \param actionGroup Action group that is unknown.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request.
		 */
		void handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Handles all unknown action requests.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that is unknown.
		 * \param data Data sent with the action request.
		 */
		void handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/**
		 * \brief Sends encoded reply to client.
		 * \param data Data which is sent as a reply to the client.
		 */
		void sendReply(const QByteArray& data);
		
		/** \brief Stores sphere data. */
		SphereManager sphMan;
		
		/** \brief Stores sphere data. */
		SphereCalculator& sphCalc;
		
	public:
		/**
		 * \brief Starts a new server that handles the request from the new client.
		 * \param sock Socket of the connection to the client.
		 */
		ActionReceiver(QTcpSocket* sock);
		
		~ActionReceiver();
		
	public slots:
		/** \brief Reads data from client. */
		void readData();
	};
	
}

#endif