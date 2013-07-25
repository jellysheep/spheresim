
#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <QtGlobal>
#include <QString>

namespace SphereSim{
	
	/*
	 * namespace Connection:
	 * Specifies default connection details for client and server. 
	 */
	namespace Connection{
		const QString defaultAddress = "127.0.0.1";
		const quint16 defaultPort = 8765;
		const char startByte = '<', endByte = '>';
	}
	
}

#endif
