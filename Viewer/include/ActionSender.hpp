
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
#include <Sphere.hpp>

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
		quint16 connectionTryCount;
		
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
		void sendAction(quint8 actionGroup, quint8 action, QByteArray& data);
		/**
		 * \brief Sends an action request to the server.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 */
		void sendAction(quint8 actionGroup, quint8 action);
		
		/**
		 * \brief Sends an action request and data to the server and returns server reply.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 * \param data Data that will be sent with the action request.
		 * \return Reply data from the server.
		 */
		QByteArray sendReplyAction(quint8 actionGroup, quint8 action, QByteArray& data);
		/**
		 * \brief Sends an action request to the server and returns server reply.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action.
		 * \return Reply data from the server.
		 */
		QByteArray sendReplyAction(quint8 actionGroup, quint8 action);
		
	public:
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(QHostAddress addr, quint16 port);
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(QString addr, quint16 port);
		/**
		 * \brief Starts a QTcpSocket with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ActionSender(const char* addr, quint16 port);
		
		~ActionSender();
		
		/**
		 * \brief Asks server for the version number and returns it.
		 * \return The version string reported by the server.
		 */
		QString getVersion();
		
		/**
		 * \brief Asks server for a "true" string and returns it.
		 * \return The "true" string sent by the server.
		 */
		QString getTrueString();
		
		/**
		 * \brief Shows if socket is connected.
		 * \return True if connected, false if not.
		 */
		bool isConnected();
		
		/**
		 * \brief Requests the server to add one sphere.
		 * \return Current sphere count reported by server.
		 */
		quint16 addSphere();
		
		/**
		 * \brief Requests the server to remove the last sphere.
		 * \return Current sphere count reported by server.
		 */
		quint16 removeLastSphere();
		
		/**
		 * \brief Requests the server to send the current sphere count.
		 * \return Current sphere count reported by server.
		 */
		quint16 getSphereCount();
		
		/**
		 * \brief Requests the server to update one sphere.
		 * \param i Index of the sphere to update.
		 * \param s Sphere data to update.
		 */
		void updateSphere(quint16 i, Sphere s);
		
		/**
		 * \brief Requests the server to send one sphere.
		 * \param i Index of the sphere to send.
		 * \param s Sphere data to update.
		 */
		void getSphere(quint16 i, Sphere& s);
		
		/**
		 * \brief Requests the server to send one full sphere.
		 * \param i Index of the sphere to send.
		 * \param s Sphere data to update.
		 */
		void getFullSphere(quint16 i, Sphere& s);
		
		/**
		 * \brief Requests the server to calculate one step.
		 * \return Time in ms needed to calculate the step.
		 */
		quint32 calculateStep();
		
		/**
		 * \brief Requests the server to set the time step.
		 * \param timeStep Requested time step in seconds.
		 */
		void setTimeStep(Scalar timeStep);
		
		/**
		 * \brief Requests the server to send the time step.
		 * \return Sent time step in seconds.
		 */
		Scalar getTimeStep();
		
		/**
		 * \brief Requests the server to set the integrator method.
		 * \param integratorMethod Requested integrator method.
		 */
		void setIntegratorMethod(quint8 integratorMethod);
		
		/**
		 * \brief Requests the server to send the integrator method.
		 * \return Sent integrator method.
		 */
		quint8 getIntegratorMethod();
		
		/**
		 * \brief Asks server for the floating type and returns it.
		 * \return The floating type string sent by the server.
		 */
		QString getFloatingType();
		
		/**
		 * \brief Requests the server to send the used calculation steps.
		 * \return Sent used calculation steps.
		 */
		quint32 popCalculationCounter();
		
		/**
		 * \brief Requests the server to calculate some steps.
		 * \param steps Number of steps to calculate.
		 * \return Time in ms needed to calculate the steps.
		 */
		quint32 calculateSomeSteps(quint32 steps);
		
		/**
		 * \brief Requests the server to send the total energy.
		 * \return Sent total energy.
		 */
		Scalar getTotalEnergy();
	
	public slots:
		void connected(){
			connectedFlag = true;
		}
	};
	
}

#endif
