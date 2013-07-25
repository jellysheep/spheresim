
#ifndef _ACTIONSERVER_HPP_
#define _ACTIONSERVER_HPP_

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>

class QTcpServer;
class QHostAddress;

namespace SphereSim{
	
	/*
	 * class ActionServer:
	 * Starts server and waits for incoming connections from clients.
	 * Replies to client requests.
	 */
	class ActionServer:private QObject{
		Q_OBJECT
	private:
		QTcpServer* server;
	public:
		/*
		 * constructor:
		 * Starts a QTcpServer and listens to the specified port.
		 */
		ActionServer(const QHostAddress& addr, const quint16 port);
		ActionServer(const QString& addr, const quint16 port);
		ActionServer(const char* addr, const quint16 port);
		
	public slots:
		/*
		 * slot newConnection:
		 * Receives connection from client and creates a new ActionReceiver for it. 
		 */
		void newConnection();
	};
	
}

#endif
