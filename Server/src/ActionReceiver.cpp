
#include <ActionReceiver.hpp>

#include <QTcpSocket>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* sock){
	qDebug()<<"ActionReceiver: constructor called";
	answeringRequest = false;
	socket = sock;
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
}

void ActionReceiver::readData(){
	qDebug()<<"ActionReceiver: new data available";
	if(!answeringRequest){
		char* data = new char[3];
		qint64 result = socket->read(data, 3);
		if(result == 3){
			if(data[0] == 1 && data[1] == 2 && data[2] == 3){
				sendVersion();
			}
		}
	}
}

void ActionReceiver::sendVersion(){
	socket->write("2.0.01");
}

void ActionReceiver::disconnected(){
	qDebug()<<"ActionReceiver: disconnected";
}
