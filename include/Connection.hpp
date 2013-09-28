/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <QtGlobal>
#include <QString>

namespace SphereSim{
	
	/**
	 * \brief Specifies default connection details for client and server. 
	 */
	namespace Connection{
		extern QString defaultListeningAddress;
		extern QString defaultAddress;
		extern quint16 defaultPort;
		extern char startByte, endByte;
	}
	
}

#endif
