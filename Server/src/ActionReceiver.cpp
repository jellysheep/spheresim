
#include <ActionReceiver.hpp>
#include <Version.hpp>
#include <Connection.hpp>

#include <QTcpSocket>

using namespace SphereSim;

ActionReceiver::ActionReceiver(QTcpSocket* sock){
	collectingRequestData = false;
	socket = sock;
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
}

void ActionReceiver::readData(){
	QByteArray arr = socket->readAll();
	
	processData(arr);
}

void ActionReceiver::processData(QByteArray arr){
	int endIndex, startIndex;
	endIndex = arr.indexOf(Connection::endByte);
	startIndex = arr.indexOf(Connection::startByte);
	
	if(endIndex<0){
		if(startIndex<0){
			///no endByte or startByte
			if(collectingRequestData){
				qDebug()<<"ActionReceiver: appending data to array";
				requestData.append(arr);
			}
		}else{
			///only startByte
			if(!collectingRequestData){
				qDebug()<<"ActionReceiver: creating new data array";
				//what if last request did not end correctly? next request would be skipped (waiting for endByte)...
				collectingRequestData = true;
				requestData = arr.right(arr.size()-startIndex-1);
			}
		}
	}else{
		if(startIndex<0){
			///only endByte
			if(collectingRequestData){
				qDebug()<<"ActionReceiver: appending data and finishing array";
				requestData.append(arr.left(endIndex));
				collectingRequestData = false;
				processRequest();
			}
		}else{
			///startByte and endByte
			if(startIndex<endIndex){
				///startByte before endByte
				qDebug()<<"ActionReceiver: creating and finishing new data array";
				requestData = arr.mid(startIndex+1, endIndex-startIndex-1);
				collectingRequestData = false;
				processRequest();
				processData(arr.right(arr.size()-endIndex-1));
			}else{
				///endByte before startByte
				if(collectingRequestData){
					qDebug()<<"ActionReceiver: finishing and creating new data array";
					requestData.append(arr.left(endIndex));
					collectingRequestData = false;
					processRequest();
					processData(arr.right(arr.size()-endIndex-1));
				}
			}
		}
	}
}

void ActionReceiver::processRequest(){
	QByteArray data = QByteArray::fromBase64(requestData);
	char actionGroup = data[0];
	char action = data[1];
	if(data.size()>2){
		data = data.right(data.length()-2);
	}else{
		data.clear();
	}
	if(data.size()<50){
		qDebug()<<"ActionReceiver: receiving"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<data<<Connection::endByte;
	}else{
		qDebug()<<"ActionReceiver: receiving"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<"[data]"<<Connection::endByte;
	}
	handleBasicAction(actionGroup, action, data);
}

void ActionReceiver::handleAction(const char actionGroup, const char action, const QByteArray data){
	switch(actionGroup){
	case ActionGroups::basic:
		handleBasicAction(actionGroup, action, data);
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleBasicAction(const char actionGroup, const char action, const QByteArray data){
	switch(action){
	case BasicActions::getVersion:
		sendVersion();
		break;
	default:
		handleUnknownAction(actionGroup, action, data);
		break;
	}
}

void ActionReceiver::handleUnknownAction(const char actionGroup, const char action, const QByteArray data){
	qWarning()<<"ActionReceiver: Warning: received unknown action group or action ("<<(int)actionGroup<<(int)action<<")";
}

void ActionReceiver::sendReply(const QByteArray& arr){
	QByteArray data = arr.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
	if(arr.size()<50){
		qDebug()<<"ActionReceiver: sending"<<Connection::startByte<<arr<<Connection::endByte<<data;
	}else{
		qDebug()<<"ActionReceiver: sending"<<Connection::startByte<<"[data]"<<Connection::endByte;
	}
}

void ActionReceiver::sendVersion(){
	sendReply("SphereSim Server v" VERSION_STR);
}

void ActionReceiver::disconnected(){
	qDebug()<<"ActionReceiver: disconnected";
}
