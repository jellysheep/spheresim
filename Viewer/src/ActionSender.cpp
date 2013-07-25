
#include <ActionSender.hpp>

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

void ActionSender::sendAction(const char actionGroup, const char action, const QByteArray& data){
	socket->putChar(actionGroup);
	socket->putChar(action);
	socket->write(data);
}
void ActionSender::sendAction(const char actionGroup, const char action){
	const static QByteArray arr;
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
