
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

const QString ActionSender::getVersion(){
	const static char data[] = {1,2,3};
	socket->write(data);
	char* retData = new char[6];
	socket->waitForReadyRead();
	qint64 result = socket->read(retData, 6);
	if(result == 6){
		return retData;
	}
	return "error";
}
