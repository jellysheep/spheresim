/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_

#include <Actions.hpp>
#include <Sphere.hpp>
#include <FrameBuffer.hpp>
#include <SimulatedSystem.hpp>

#include <QtGlobal>
#include <QObject>
#include <QAbstractSocket>
#include <QProcess>
#include <QElapsedTimer>

class QTcpSocket;
class QHostAddress;

namespace SphereSim
{
	/** \brief Start client, build up server connection, send request to server. */
	class ActionSender : public QObject
	{
		Q_OBJECT

	private:
		/** \brief Server address. */
		QHostAddress* addr;

		/** \brief Server port. */
		quint16 port;

		/** \brief Server connection socket. */
		QTcpSocket* socket;

		/** \brief Flag for connection to server. */
		bool connectedFlag;

		/** \brief Number of server connection tries. */
		quint16 connectionTryCount;

		/** \brief Process of the automatically started server. */
		QProcess serverProcess;

		/** \brief Flag for automatically started server. */
		bool createdOwnServer;

		/** \brief Buffer for the spheres received from server. */
		FrameBuffer<Sphere> frameBuffer;

		/** \brief Last received server status. */
		quint8 lastServerStatus;

		/** \brief Flag for server replies. */
		bool receivedServerReply;

		/** \brief Last server reply data. */
		QByteArray lastServerReplyData;

		/** \brief Partial server reply data. */
		QByteArray replyData;

		/** \brief Flag for currently collecting server data.
		 * If true, no new replies are accepted. */
		bool collectingReplyData;

		/** \brief Timer used to measure framerates. */
		QElapsedTimer framerateTimer;

		/** \brief Number of received frames since last counter reset. */
		quint32 frameCounter;

		/** \brief Measured rate of received frames per second. */
		Scalar receivedFramesPerSecond;

		/** \brief Process received reply data.
		 * \param byteArray Received data to process. */
		void processData(QByteArray byteArray);

		/** \brief Process and answer to received reply. */
		void processReply();

		/** \brief Send an action request to the server.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action. */
		void sendAction(quint8 actionGroup, quint8 action);

		/** \copydoc sendAction
		 * \param data Data that will be sent with the action request. */
		void sendAction(quint8 actionGroup, quint8 action, QByteArray& data);

		/** \brief Send an action request to the server and return server reply.
		 * \return Reply data from the server.
		 * \param actionGroup Group of the requested action.
		 * \param action Requested action. */
		QByteArray sendReplyAction(quint8 actionGroup, quint8 action);

		/** \copydoc sendReplyAction
		 * \param data Data that will be sent with the action request. */
		QByteArray sendReplyAction(quint8 actionGroup, quint8 action,
			QByteArray& data);

		/** \brief Update sphere number and resize frame buffer.
		 * \param sphereCount Number of spheres. */
		void updateSphereCount(quint16 sphereCount);

		void willBeSimulating();

	public:
		/** \brief Start an ActionSender with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to. */
		ActionSender(QStringList args, QHostAddress addr, quint16 port,
			QObject* client);

		~ActionSender();

		ActionSender() = delete;
		ActionSender(const ActionSender&) = delete;
		ActionSender& operator=(const ActionSender&) = delete;

		bool failureExitWhenDisconnected;

		SimulatedSystem* simulatedSystem;

		/** \return Server connection flag.
		 * \see connectedFlag */
		bool isConnected();

		FrameBuffer<Sphere>* getFrameBuffer();

		/** \copydoc SpheresUpdatingActions::updateSphere
		 * \param i Sphere index.
		 * \param s Sphere data. */
		void updateSphere(quint16 i, Sphere s);

		/** \copydoc SpheresUpdatingActions::getBasicSphereData
		 * \copydetails updateSphere */
		void getBasicSphereData(quint16 i, Sphere& s);

		/** \copydoc SpheresUpdatingActions::getAllSphereData
		 * \copydetails updateSphere */
		void getAllSphereData(quint16 i, Sphere& s);

		/** \copydoc SpheresUpdatingActions::addSomeSpheres
		 * \copydetails getSphereCount */
		quint16 addSomeSpheres(quint16 sphCount);

		/** \copydoc SpheresUpdatingActions::removeSomeLastSpheres
		 * \copydetails getSphereCount */
		quint16 removeSomeLastSpheres(quint16 sphCount);

		/** \copydoc SpheresUpdatingActions::updateSpherePositionsInBox */
		void updateSpherePositionsInBox(Scalar randomDisplacement,
			Scalar randomSpeed);

		/** \copydoc SpheresUpdatingActions::updateAllSpheres
		 * \param s Sphere data. */
		void updateAllSpheres(Sphere s);

		/** \copydoc SpheresUpdatingActions::updateKineticEnergy
		 * \param factor Kinetic energy scale factor. */
		void updateKineticEnergy(Scalar factor);

		/** \copydoc CalculationActions::calculateStep */
		void calculateStep();

		/** \copydoc CalculationActions::popCalculationCounter
		 * \return Numer of force calculations per step. */
		quint32 popCalculationCounter();

		/** \copydoc CalculationActions::calculateSomeSteps
		 * \param steps Number of steps to calculate (0 = unlimited). */
		void calculateSomeSteps(quint32 steps);

		/** \copydoc CalculationActions::popStepCounter
		 * \return Number of simulatied time steps. */
		quint32 popStepCounter();

		/** \copydoc CalculationActions::getLastStepCalculationTime */
		quint32 getLastStepCalculationTime();

		/** \copydoc InformationActions::getTotalEnergy
		 * \return Total energy (in joules). */
		Scalar getTotalEnergy();

		/** \copydoc InformationActions::getKineticEnergy
		 * \return Kinetic energy (in joules). */
		Scalar getKineticEnergy();

	public slots:
		/** Set connectedFlag to true.
		 * \see connectedFlag */
		void connected()
		{
			connectedFlag = true;
		}

		void disconnected();

		/** \brief Read data from server. */
		void readData();

		/** \brief Executed when some measured event (e.g. received frame) happened.
		 * \see newFrameReceived */
		void framerateEvent();

		/** \copydoc SpheresUpdatingActions::addSphere
		 * \copydetails getSphereCount */
		quint16 addSphere();

		/** \copydoc SpheresUpdatingActions::removeLastSphere
		 * \copydetails getSphereCount */
		quint16 removeLastSphere();

		/** \copydoc CalculationActions::startSimulation */
		void startSimulation();

		/** \copydoc CalculationActions::stopSimulation */
		void stopSimulation();

		void sendVariable(QByteArray variableToSend);

		void variableUpdated(int var);

	signals:
		/** \brief New frame received from server. */
		void newFrameReceived();

		/** \brief Executed when framerates have been updated.
		 * \see framerateEvent */
		void framerateUpdate();

		/** \brief Percentage level of FrameBuffer was updated.
		 * \param percentageLevel Updated percentage level. */
		void frameBufferPercentageLevelUpdate(int percentageLevel);

		/** \brief Percentage level of FrameBuffer changed greatly.
		 * \copydetails frameBufferPercentageLevelUpdate */
		void greatFrameBufferPercentageLevelUpdate(int percentageLevel);

		/** \brief Number of spheres changed. */
		void sphereCountChanged(quint16 sphereCount);

		/** \brief Number of spheres changed. */
		void sphereCountChangedDouble(double sphereCount);

		/** \brief Server connection was closed. */
		void connectionClosed();
	};

}

#endif
