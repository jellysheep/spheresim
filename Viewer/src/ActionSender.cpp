
#include <ActionSender.hpp>
#include <Connection.hpp>

#include <QTcpSocket>
#include <QHostAddress>

using namespace SphereSim;

ActionSender::ActionSender(const QHostAddress& addr, const quint16 port){
	qDebug()<<"ActionSender: constructor called";
	socket = new QTcpSocket();
	socket->connectToHost(addr, port);
}
ActionSender::ActionSender(const QString& addr, const quint16 port)
	:ActionSender(QHostAddress(addr),port){
}
ActionSender::ActionSender(const char* addr, const quint16 port)
	:ActionSender(QString(addr),port){
}

void ActionSender::sendAction(const char actionGroup, const char action, const QByteArray& arr){
	QByteArray data;
	data.append(actionGroup);
	data.append(action);
	data.append(arr);
	if(arr.size()<50){
		qDebug()<<"ActionSender: sending raw"<<data;
	}
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

const QString ActionSender::getVersion(){
	sendAction(ActionGroups::basic, BasicActions::getVersion);
	const static int length = 40;
	char* retData = new char[length];
	socket->waitForReadyRead();
	qint64 result = socket->read(retData, length);
	if(result > 0){
		return QByteArray(retData, result);
	}
	return QString("error");
}
