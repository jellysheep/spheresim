/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _MESSAGETRANSMITTER_HPP_
#define _MESSAGETRANSMITTER_HPP_

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <nanomsg/nn.hpp>
#include <sstream>
#include <string>

namespace SphereSim
{
    /** \brief Sending and receiving encoded messages over network. */
    class MessageTransmitter : public QObject
    {
        Q_OBJECT

    private:
        nn::socket* sendSocket;
        nn::socket* recvSocket;

        QTimer timer;

        QElapsedTimer elapsedTimer;

        bool connected;

        /** \brief Base64-encode message. */
        std::string encode(const std::string& data);

        /** \brief Base64-decode message. */
        std::string decode(const std::string& data);

    public:
        MessageTransmitter(nn::socket* sendSocket, nn::socket* recvSocket);

        MessageTransmitter() = delete;
        MessageTransmitter(const MessageTransmitter&) = delete;
        MessageTransmitter& operator=(const MessageTransmitter&) = delete;

        void start();

    public slots:
        void send(std::string data);

        /** \brief Read new available data. */
        void readData();

    signals:
        void processData(std::string data);

        void receiveTimeout();

    };

}

#endif /*_MESSAGETRANSMITTER_HPP_*/
