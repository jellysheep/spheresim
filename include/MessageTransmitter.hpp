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
#include <sstream>
#include <string>

class QTcpSocket;

namespace SphereSim
{
    /** \brief Sending and receiving encoded messages over network. */
    class MessageTransmitter : public QObject
    {
        Q_OBJECT

    private:
        QTcpSocket* socket;

        /** \brief Partial data. */
        std::ostringstream partialData;

        /** \brief Flag for currently collecting data.
         * If true, no new requests are accepted. */
        bool collectingData;

        /** \brief Base64-encode message. */
        std::string encode(const std::string& data);

        /** \brief Base64-decode message. */
        std::string decode(const std::string& data);

    public:
        MessageTransmitter(QTcpSocket* socket);

        MessageTransmitter() = delete;
        MessageTransmitter(const MessageTransmitter&) = delete;
        MessageTransmitter& operator=(const MessageTransmitter&) = delete;

        void send(std::string data);

    public slots:

        /** \brief Read new available data. */
        void readData();

    signals:
        void processData(std::string data);

    };

}

#endif /*_MESSAGETRANSMITTER_HPP_*/
