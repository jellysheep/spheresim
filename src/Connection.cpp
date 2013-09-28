/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <Connection.hpp>

namespace SphereSim{
	
	namespace Connection{
		QString defaultListeningAddress = "127.0.0.1";
		QString defaultAddress = "127.0.0.1";
		quint16 defaultPort = 8765;
		char startByte = '<', endByte = '>';
	}
	
}
