/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_

#include <Actions.hpp>
#include <Sphere.hpp>
#include <FrameBuffer.hpp>

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
		QByteArray sendReplyAction(quint8 actionGroup, quint8 action, QByteArray& data);
		
		/** \brief Update sphere number and resize frame buffer.
		 * \param sphereCount Number of spheres. */
		void updateSphereCount(quint16 sphereCount);
		
		/** \brief Timer used to measure framerates. */
		QElapsedTimer framerateTimer;
		
		/** \brief Number of received frames since last counter reset. */
		quint32 frameCounter;
		
		/** \brief Measured rate of received frames per second. */
		Scalar receivedFramesPerSecond;
		
	public:
		/** \brief Start an ActionSender with the specified address and port.
		 * \param args The arguments that the program was invoked with.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to. */
		ActionSender(QStringList args, QHostAddress addr, quint16 port);
		
		~ActionSender();
		
		/** \return Server connection flag.
		 * \see connectedFlag */
		bool isConnected();
		
		FrameBuffer<Sphere>* getFrameBuffer();
		
		/** \copydoc BasicActions::getServerVersion
		 * \return Version string reported by server. */
		QString getServerVersion();
		
		/** \copydoc BasicActions::getTrueString
		 * \return "True" string sent by server. */
		QString getTrueString();
		
		/** \copydoc BasicActions::getServerFloatingType
		 * \return The floating type string sent by the server. */
		QString getServerFloatingType();
		
		/** \copydoc SpheresUpdatingActions::updateSphere
		 * \param i Sphere index.
		 * \param s Sphere data. */
		void updateSphere(quint16 i, Sphere s);
		
		/** \copydoc SpheresUpdatingActions::getSphereCount
		 * \return Current sphere count reported by server. */
		quint16 getSphereCount();
		
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
		void updateSpherePositionsInBox(Scalar randomDisplacement, Scalar randomSpeed);
		
		/** \copydoc SpheresUpdatingActions::updateAllSpheres
		 * \param s Sphere data. */
		void updateAllSpheres(Sphere s);
		
		/** \copydoc CalculationActions::calculateStep */
		void calculateStep();
		
		/** \copydoc CalculationActions::updateTimeStep
		 * \param timeStep Time step in seconds. */
		void updateTimeStep(Scalar timeStep);
		
		/** \copydoc CalculationActions::getTimeStep
		 * \return Time step in seconds. */
		Scalar getTimeStep();
		
		/** \copydoc CalculationActions::updateIntegratorMethod
		 * \param integratorMethod Integrator method. */
		void updateIntegratorMethod(quint8 integratorMethod);
		
		/** \copydoc CalculationActions::getIntegratorMethod
		 * \return Integrator method. */
		quint8 getIntegratorMethod();
		
		/** \copydoc CalculationActions::popCalculationCounter
		 * \return Numer of force calculations per step. */
		quint32 popCalculationCounter();
		
		/** \copydoc CalculationActions::calculateSomeSteps
		 * \param steps Number of steps to calculate (0 = unlimited). */
		void calculateSomeSteps(quint32 steps);
		
		/** \copydoc CalculationActions::getIsSimulating
		 * \return Simulation run flag. */
		bool getIsSimulating();
		
		/** \copydoc CalculationActions::popStepCounter
		 * \return Number of simulatied time steps. */
		quint32 popStepCounter();
		
		/** \copydoc CalculationActions::updateFrameSending */
		void updateFrameSending(bool sendFramesRegularly);
		
		/** \copydoc CalculationActions::updateCollisionDetection */
		void updateCollisionDetection(bool detectCollisions);
		
		/** \copydoc CalculationActions::updateGravityCalculation */
		void updateGravityCalculation(bool calculateGravity);
		
		/** \copydoc CalculationActions::updateLennardJonesPotentialCalculation */
		void updateLennardJonesPotentialCalculation(bool calculateLennardJonesPotential);
		
		/** \copydoc CalculationActions::updateMaximumStepDivision */
		void updateMaximumStepDivision(quint16 maxStepDivisionNumber);
		
		/** \copydoc CalculationActions::updateMaximumStepError */
		void updateMaximumStepError(Scalar maxStepError);
		
		/** \copydoc CalculationActions::getLastStepCalculationTime */
		quint32 getLastStepCalculationTime();
		
		/** \copydoc InformationActions::getTotalEnergy
		 * \return Total energy (in joules). */
		Scalar getTotalEnergy();
		
		/** \copydoc InformationActions::getKineticEnergy
		 * \return Kinetic energy (in joules). */
		Scalar getKineticEnergy();
		
		/** \copydoc SimulatedSystemActions::updateSphereE
		 * \param E_sphere Sphere E modulus. */
		void updateSphereE(Scalar E_sphere);
		
		/** \copydoc SimulatedSystemActions::updateSpherePoissonRatio
		 * \param poisson_sphere Sphere poisson number. */
		void updateSpherePoissonRatio(Scalar poisson_sphere);
		
		/** \copydoc SimulatedSystemActions::updateWallE
		 * \param E_wall Requested wall E modulus. */
		void updateWallE(Scalar E_wall);
		
		/** \copydoc SimulatedSystemActions::updateWallPoissonRatio
		 * \param poisson_wall Requested wall poisson number. */
		void updateWallPoissonRatio(Scalar poisson_wall);
		
		/** \copydoc SimulatedSystemActions::updateEarthGravity
		 * \param earthGravity Requested earth gravity. */
		void updateEarthGravity(Vector3 earthGravity);
		
		/** \copydoc SimulatedSystemActions::updateGravitationalConstant
		 * \param G Gravitational constant G. */
		void updateGravitationalConstant(Scalar G);
		
		/** \copydoc SimulatedSystemActions::updateBoxSize
		 * \param boxSize Requested box size. */
		void updateBoxSize(Vector3 boxSize);
		
		/** \copydoc SimulatedSystemActions::updateKineticEnergy
		 * \param factor Kinetic energy scale factor. */
		void updateKineticEnergy(Scalar factor);
		
		/** \copydoc SimulatedSystemActions::updateKineticEnergy */
		void updateTargetTemperature(Scalar targetTemperature);
		
		/** \copydoc SimulatedSystemActions::updatePeriodicBoundaryConditions */
		void updatePeriodicBoundaryConditions(bool periodicBoundaryConditions);
	
	public slots:
		/** Set connectedFlag to true. 
		 * \see connectedFlag */
		void connected()
		{
			connectedFlag = true;
		}
		
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
	};
	
}

#endif
