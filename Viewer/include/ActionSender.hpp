
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
		
		/*
		 * method sendAction:
		 * Sends action request and data to server. 
		 */
		void sendAction(const char actionGroup, const char action, const QByteArray& data);
		void sendAction(const char actionGroup, const char action);
		
		/*
		 * method sendReplyAction:
		 * Sends action request and data to server and return server reply. 
		 */
		QByteArray sendReplyAction(const char actionGroup, const char action, const QByteArray& data);
		QByteArray sendReplyAction(const char actionGroup, const char action);
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
