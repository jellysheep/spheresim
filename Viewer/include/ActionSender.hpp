
#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_

#include <Actions.hpp>

#include <QtGlobal>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	/*
	 * class ActionSender:
	 * Starts client and builds up connection to server.
	 * Sends actions to server when requested.
	 */
	class ActionSender{
	private:
		QTcpSocket* socket;
	public:
		/*
		 * constructor:
		 * Starts a QTcpSocket with the specified address and port.
		 */
		ActionSender(const QHostAddress& addr, const quint16 port);
		ActionSender(const QString& addr, const quint16 port);
		ActionSender(const char* addr, const quint16 port);
		
		/*
		 * method getVersion:
		 * Asks server for the version number and returns it.
		 */
		const QString getVersion();
	};
	
}

#endif
