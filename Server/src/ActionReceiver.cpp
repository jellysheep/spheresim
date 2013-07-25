
#include <ActionReceiver.hpp>
#include <Version.hpp>

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
		answeringRequest = true;
		char* data = new char[2];
		qint64 result = socket->read(data, 2);
		if(result >= 2){
			switch(data[0]){
			case ActionGroups::basic:
				handleBasicAction(data);
				break;
			default:
				handleUnknownAction(data);
				break;
			}
		}
		answeringRequest = false;
	}
}

void ActionReceiver::handleBasicAction(const char* data){
	switch(data[1]){
	case BasicActions::getVersion:
		sendVersion();
		break;
	default:
		handleUnknownAction(data);
		break;
	}
}

void ActionReceiver::handleUnknownAction(const char* data){
	
}

void ActionReceiver::sendVersion(){
	socket->write("SphereSim Server v" VERSION_STR);
}

void ActionReceiver::disconnected(){
	qDebug()<<"ActionReceiver: disconnected";
}
