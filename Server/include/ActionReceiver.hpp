
#ifndef _ACTIONRECEIVER_HPP_
#define _ACTIONRECEIVER_HPP_

#include <Actions.hpp>

#include <QtGlobal>
#include <QObject>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	/*
	 * class ActionReceiver:
	 * Takes client socket and replies to client requests.
	 */
	class ActionReceiver:QObject{
		Q_OBJECT
		
	private:
		QTcpSocket* socket;
		
		/* bool answeringRequest:
		 * Flag if currently a client request is answered.
		 * If true, no new requests are accepted. */
		bool answeringRequest;
		
	public:
		/*
		 * constructor:
		 * Takes client socket. 
		 */
		ActionReceiver(QTcpSocket* sock);
		
		/*
		 * method sendVersion:
		 * Returns server version to the client.
		 */
		void sendVersion();
		
	public slots:
		/*
		 * slot readData:
		 * Reads data from client.
		 */
		void readData();
		
		/*
		 * slot disconnected:
		 * Executed when connection was released.
		 */
		void disconnected();
	};
	
}

#endif
