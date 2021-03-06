/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_

#include "Actions.hpp"
#include "Sphere.hpp"
#include "FrameBuffer.hpp"
#include "SimulatedSystem.hpp"

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <nanomsg/nn.hpp>
#include <string>

namespace SphereSim
{
    class MessageTransmitter;

    /** \brief Start client, build up server connection, send request to server. */
    class ActionSender : public QObject
    {
        Q_OBJECT

    private:
        /** \brief Socket sending to server. */
        nn::socket sendSocket;

        /** \brief Socket receiving from server. */
        nn::socket recvSocket;

        /** \brief Flag for connection to server. */
        bool connectedFlag;

        /** \brief Client ID for communication with server. */
        unsigned int clientID;

        /** \brief Buffer for the spheres received from server. */
        FrameBuffer<Sphere> frameBuffer;

        /** \brief Last received server status. */
        unsigned short lastServerStatus;

        /** \brief Flag for server replies. */
        bool receivedServerReply;

        /** \brief Last server reply data. */
        std::string lastServerReplyData;

        /** \brief Timer used to measure framerates. */
        QElapsedTimer framerateTimer;

        /** \brief Number of received frames. */
        unsigned int frameCounter;

        /** \brief Number of received frames until last second. */
        unsigned int oldFrameCounter;

        /** \brief Measured rate of received frames per second. */
        Scalar receivedFramesPerSecond;

        /** \brief Encapsulate and encode messages sent over network. */
        MessageTransmitter* messageTransmitter;

        /** \brief Server and client are connected and ready. */
        bool readyToRun;

        /** \brief Heartbeat sending timer. */
        QTimer heartbeatTimer;

        /** \brief Send an action request to the server.
         * \param actionGroup Group of the requested action.
         * \param action Requested action. */
        void sendAction(unsigned char actionGroup, unsigned char action);

        /** \copydoc sendAction
         * \param data Data that will be sent with the action request. */
        void sendAction(unsigned char actionGroup, unsigned char action,
            const std::string& data);

        /** \brief Send an action request to the server and return server reply.
         * \return Reply data from the server.
         * \param actionGroup Group of the requested action.
         * \param action Requested action. */
        std::string sendReplyAction(unsigned char actionGroup, unsigned char action);

        /** \copydoc sendReplyAction
         * \param data Data that will be sent with the action request. */
        std::string sendReplyAction(unsigned char actionGroup, unsigned char action,
            const std::string& data);

        /** \brief Update sphere number and resize frame buffer.
         * \param sphereCount Number of spheres. */
        void updateSphereCount(unsigned short sphereCount);

        void willBeSimulating();

    public:
        /** \brief Start an ActionSender with the specified address and port.
         * \param addr The address that the socket will be connecting to.
         * \param sendPort The port that the client will be sending to.
         * \param recvPort The port that the client will be listening to. */
        ActionSender(const char* addr, unsigned short sendPort,
            unsigned short recvPort, QObject* client);

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

        /** \copydoc BasicActions::terminateServer */
        void terminateServer();

        /** \copydoc SpheresUpdatingActions::updateSphere
         * \param i Sphere index.
         * \param s Sphere data. */
        void updateSphere(unsigned short i, Sphere s);

        /** \copydoc SpheresUpdatingActions::getBasicSphereData
         * \copydetails updateSphere */
        void getBasicSphereData(unsigned short i, Sphere& s);

        /** \copydoc SpheresUpdatingActions::getAllSphereData
         * \copydetails updateSphere */
        void getAllSphereData(unsigned short i, Sphere& s);

        /** \copydoc SpheresUpdatingActions::addSomeSpheres
         * \copydetails getSphereCount */
        unsigned short addSomeSpheres(unsigned short sphCount);

        /** \copydoc SpheresUpdatingActions::removeSomeLastSpheres
         * \copydetails getSphereCount */
        unsigned short removeSomeLastSpheres(unsigned short sphCount);

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
        unsigned int popCalculationCounter();

        /** \copydoc CalculationActions::calculateSomeSteps
         * \param steps Number of steps to calculate (0 = unlimited). */
        void calculateSomeSteps(unsigned int steps);

        /** \copydoc CalculationActions::popStepCounter
         * \return Number of simulatied time steps. */
        unsigned int popStepCounter();

        /** \copydoc CalculationActions::getLastStepCalculationTime */
        unsigned int getLastStepCalculationTime();

        /** \copydoc InformationActions::getTotalEnergy
         * \return Total energy (in joules). */
        Scalar getTotalEnergy();

        /** \copydoc InformationActions::getKineticEnergy
         * \return Kinetic energy (in joules). */
        Scalar getKineticEnergy();

    public slots:
        /** \brief Process and answer to received reply. */
        void processReply(std::string data);

        /** \brief Executed when some measured event (e.g. received frame) happened.
         * \see newFrameReceived */
        void framerateEvent();

        /** \copydoc SpheresUpdatingActions::addSphere
         * \copydetails getSphereCount */
        unsigned short addSphere();

        /** \copydoc SpheresUpdatingActions::removeLastSphere
         * \copydetails getSphereCount */
        unsigned short removeLastSphere();

        /** \copydoc CalculationActions::startSimulation */
        void startSimulation();

        /** \copydoc CalculationActions::stopSimulation */
        void stopSimulation();

        void sendVariable(std::string variableToSend);

        void variableUpdated(int var);

        void heartbeat();

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
        void sphereCountChanged(unsigned short sphereCount);

        /** \brief Number of spheres changed. */
        void sphereCountChangedDouble(double sphereCount);

        /** \brief Server connection was closed. */
        void connectionClosed();

        void serverReady();
    };

}

#endif /*_ACTIONSENDER_HPP_*/
