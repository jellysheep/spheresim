
#ifndef _ACTIONRECEIVER_HPP_
#define _ACTIONRECEIVER_HPP_

#include <Actions.hpp>

#include <QObject>

class QTcpSocket;
class QHostAddress;

namespace SphereSim{
	
	class SphereManager;
	
	/*
	 * class ActionReceiver:
	 * Takes client socket and replies to client requests.
	 */
	class ActionReceiver:public QObject{
		Q_OBJECT
		
	private:
		/* field socket:
		 * Holds the socket of the connection. */
		QTcpSocket* socket;
		
		/* field requestData:
		 * Collected data from a client request. */
		QByteArray requestData;
		
		/* field collectingRequestData:
		 * Flag if currently data from a client request is being collected.
		 * If true, no new requests are accepted. */
		bool collectingRequestData;
		
		/* method processData:
		 * Processes received request data. */
		void processData(QByteArray arr);
		
		/* method processRequest:
		 * Processes and answers received request. */
		void processRequest();
		
		/* method handleAction:
		 * Handles any action request. */
		void handleAction(const char actionGroup, const char action, const QByteArray data);
		
		/* method handleBasicAction:
		 * Handles basic action requests. */
		void handleBasicAction(const char actionGroup, const char action, const QByteArray data);
		
		/* method handleUnknownAction:
		 * Handles unknown action requests. */
		void handleUnknownAction(const char actionGroup, const char action, const QByteArray data);
		
		/* method sendReply:
		 * Sends encoded reply to client. */
		void sendReply(const QByteArray& data);
		
		/* field sphMan:
		 * Stores sphere data. */
		SphereManager* sphMan;
		
	public:
		/* constructor:
		 * Takes client socket. */
		ActionReceiver(QTcpSocket* sock);
		
		~ActionReceiver();
		
	public slots:
		/* slot readData:
		 * Reads data from client. */
		void readData();
	};
	
}

#endif
