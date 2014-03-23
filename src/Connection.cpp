/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <Connection.hpp>

namespace SphereSim{

	namespace Connection{
		QString listeningAddress = "127.0.0.1";
		QString address = "127.0.0.1";
		quint16 port = 8765;
		char startByte = '<', endByte = '>';
	}

}
