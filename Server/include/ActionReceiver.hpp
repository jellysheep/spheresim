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
		
		/** \brief Handle any action request and forward to specific handlers.
		 * \param actionGroup Action group that will be handled.
		 * \param action Action that will be handled.
		 * \param data Data sent with the action request. */
		void handleAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle BasicActions requests. 
		 * \copydetails handleAction */
		void handleBasicAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle SpheresUpdatingActions requests. 
		 * \copydetails handleAction */
		void handleSpheresUpdatingAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle CalculationActions requests. 
		 * \copydetails handleAction */
		void handleCalculationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle InformationActions requests. 
		 * \copydetails handleAction */
		void handleInformationAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle PhysicalConstantsActions requests. 
		 * \copydetails handleAction */
		void handlePhysicalConstantsAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle unknown action group requests. 
		 * \copydetails handleAction */
		void handleUnknownActionGroup(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Handle unknown action requests. 
		 * \copydetails handleAction */
		void handleUnknownAction(quint8 actionGroup, quint8 action, QByteArray data);
		
		/** \brief Send encoded reply to client.
		 * \param serverStatus Server status to be sent.
		 * \param data Reply data to be sent to client. */
		void sendReply(quint8 serverStatus, const QByteArray& data);
		
		/** \brief Storage and calculator of sphere data. */
		SphereCalculator sphCalc;
		
	public:
		/** \brief Start a new server handling requests from the client.
		 * \param sock Socket of the connection to the client. */
		ActionReceiver(QTcpSocket* sock);
		
		/** \brief Clean up member variables. */
		~ActionReceiver();
		
	public slots:
		/** \brief New data available: Read data from client. */
		void readData();
	};
	
}

#endif
