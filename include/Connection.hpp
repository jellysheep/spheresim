/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <QtGlobal>
#include <QString>

namespace SphereSim
{

    /** \brief Connection details for client and server. */
    namespace Connection
    {
        /** \brief The address the server is listening to. */
        extern QString listeningAddress;
        /** \brief The address the client is connecting to. */
        extern QString address;
        /** \brief The port that client and server use. */
        extern quint16 port;
        /** \brief The byte used to start an encoded request. */
        extern char startByte;
        /** \brief The byte used to end an encoded request. */
        extern char endByte;
    }

}

#endif
