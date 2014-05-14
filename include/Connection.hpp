/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

namespace SphereSim
{

    /** \brief Connection details for client and server. */
    namespace Connection
    {
        /** \brief The address the server listens to. */
        extern const char* listeningAddress;
        /** \brief The address the client connects to. */
        extern const char* address;
        /** \brief The port that the client listens to. */
        extern unsigned short serverSendPort;
        /** \brief The port that the server listens to. */
        extern unsigned short serverRecvPort;
        /** \brief The byte used to start an encoded request. */
        extern char startByte;
        /** \brief The byte used to end an encoded request. */
        extern char endByte;
    }

}

#endif /*_CONNECTION_HPP_*/
