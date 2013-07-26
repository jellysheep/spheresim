
#include <ActionSender.hpp>
#include <Connection.hpp>
#include <Console.hpp>

#include <QTcpSocket>
#include <QHostAddress>
#include <QProcess>

using namespace SphereSim;

ActionSender::ActionSender(const QHostAddress& a, const quint16 p){
	qDebug()<<"ActionSender: constructor called";
	addr = new QHostAddress(a);
	port = p;
	connectedFlag = false;
	createdOwnServer = false;
	socket = new QTcpSocket();
	connect(socket, SIGNAL(connected()), SLOT(connected()));
	connectionTryCount = 0;
	while(connectionTryCount<1000 && !connectedFlag){
		socket->connectToHost(*addr, port);
		connectionTryCount++;
		socket->waitForConnected(100);
		if(!connectedFlag){
			qDebug()<<"ActionSender: retrying to connect to host.\n";
			if(connectionTryCount<=1){
				qDebug()<<"ActionSender: starting Server.\n";
				process.start("SphereSim_Server");
				createdOwnServer = true;
			}
		}
	}
}
ActionSender::ActionSender(const QString& addr, const quint16 port)
	:ActionSender(QHostAddress(addr),port){
}
ActionSender::ActionSender(const char* addr, const quint16 port)
	:ActionSender(QString(addr),port){
}

ActionSender::~ActionSender(){
	if(createdOwnServer){
		sendReplyAction(ActionGroups::basic, BasicActions::terminateServer);
		process.waitForFinished(200);
		qDebug()<<"ActionSender: killing Server.\n";
		process.terminate();
		process.waitForFinished(200);
		process.kill();
	}
	socket->close();
	delete addr;
	delete socket;
}

void ActionSender::sendAction(const char actionGroup, const char action, const QByteArray& arr){
	QByteArray data;
	data.append(actionGroup);
	data.append(action);
	data.append(arr);
	data = data.toBase64();
	data.prepend(Connection::startByte);
	data.append(Connection::endByte);
	socket->write(data);
	if(arr.size()<50){
		qDebug()<<"ActionSender: sending"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<arr<<Connection::endByte<<data;
	}else{
		qDebug()<<"ActionSender: sending"<<Connection::startByte<<((int)actionGroup)<<((int)action)<<"[data]"<<Connection::endByte;
	}
}
void ActionSender::sendAction(const char actionGroup, const char action){
	QByteArray arr;
	sendAction(actionGroup, action, arr);
}

QByteArray ActionSender::sendReplyAction(const char actionGroup, const char action, const QByteArray& arr){
	socket->readAll(); ///clear buffer
	sendAction(actionGroup, action, arr);
	
	QByteArray retData, data;
	int endIndex, startIndex;
	bool allDataReceived = false, dataStarted = false;
	while(!allDataReceived){
		if(!socket->waitForReadyRead(1000)){
			break;
		}
		data = socket->readAll();
		if(!dataStarted){
			startIndex = data.indexOf(Connection::startByte);
			if(startIndex>=0){
				dataStarted = true;
				endIndex = data.indexOf(Connection::endByte);
				if(endIndex>=0){
					retData = data.mid(startIndex+1, endIndex-startIndex-1);
					allDataReceived = true;
					break;
				}else{
					retData = data.right(startIndex+1);
					continue;
				}
			}
		}else{
			endIndex = data.indexOf(Connection::endByte);
			if(endIndex>=0){
				retData.append(data.left(endIndex));
				allDataReceived = true;
				break;
			}else{
				retData.append(data);
				continue;
			}
		}
	}
	retData = QByteArray::fromBase64(retData);
	return retData;
}
QByteArray ActionSender::sendReplyAction(const char actionGroup, const char action){
	QByteArray arr;
	return sendReplyAction(actionGroup, action, arr);
}

const QString ActionSender::getVersion(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getVersion);
}

const QString ActionSender::getTrueString(){
	return sendReplyAction(ActionGroups::basic, BasicActions::getTrueString);
}

bool ActionSender::isConnected(){
	socket->waitForConnected(10);
	return connectedFlag;
}
