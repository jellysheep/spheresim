
#ifndef _ACTIONSENDER_HPP_
#define _ACTIONSENDER_HPP_

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>
#include <QAbstractSocket>
#include <QProcess>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	/*
	 * class ActionSender:
	 * Starts client and builds up connection to server.
	 * Sends actions to server when requested.
	 */
	class ActionSender:QObject{
		Q_OBJECT
	private:
		QHostAddress* addr;
		quint16 port;
		QTcpSocket* socket;
		bool connectedFlag;
		int connectionTryCount;
		QProcess process;
		
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
		
		~ActionSender();
		
		/* method getVersion:
		 * Asks server for the version number and returns it. */
		const QString getVersion();
		
		/* method getTrueString:
		 * Asks server for a "true" string and returns it. */
		const QString getTrueString();
		
		/* method isConnected:
		 * Shows if socket is connected. */
		bool isConnected();
	
	public slots:
		void connected(){
			connectedFlag = true;
		}
		void error(QAbstractSocket::SocketError err);
	};
	
}

#endif
