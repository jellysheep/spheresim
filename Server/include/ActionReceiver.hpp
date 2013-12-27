/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONRECEIVER_HPP_
#define _ACTIONRECEIVER_HPP_

#include <Actions.hpp>
#include <SphereCalculator.hpp>

#include <QObject>

class QTcpSocket;
class QHostAddress;

namespace SphereSim
{
	
	/** \brief Receiver of client requests. */
	class ActionReceiver:public QObject
	{
		Q_OBJECT
		
	private:
		/** \brief Socket to client. */
		QTcpSocket* socket;
		
		/** \brief Collected data from a client request. */
		QByteArray requestData;
		
		/** \brief Flag if currently data from a client request is being collected;
		 * if true, no new requests are accepted. */
		bool collectingRequestData;
		
		/** \brief Process received request data.
		 * \param byteArray Data from network stream to process. */
		void processData(QByteArray byteArray);
		
		/** \brief Process and reply to received request. */
		void processRequest();
		
		SimulatedSystem simulatedSystem;
		
		/** \brief Storage and calculator of sphere data. */
		SphereCalculator sphCalc;
		
		WorkQueue* workQueue;
		
	public:
		/** \brief Start a new server handling requests from the client.
		 * \param sock Socket of the connection to the client. */
		ActionReceiver(QTcpSocket* sock);
		
		/** \brief Clean up member variables. */
		~ActionReceiver();
		
	public slots:
		/** \brief New data available: Read data from client. */
		void readData();
		
		void sendFrame(QByteArray frameToSend);
		
		void sendVariable(QByteArray variableToSend);
		
		void simulating(bool isSimulating);
		
		/** \brief Send encoded data to client.
		 * \param serverStatus Server status to be sent.
		 * \param data Data to be sent to client. */
		void sendReply(quint8 serverStatus, QByteArray dataToSend);
		
		void terminateServer();
	};
	
}

#endif
