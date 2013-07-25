
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
	const static char data[] = {ActionGroups::basic, BasicActions::getVersion};
	socket->write(data);
	const static int length = 40;
	char* retData = new char[length];
	for(int i = 0; i<length; i++) retData[i] = 0;
	socket->waitForReadyRead();
	qint64 result = socket->read(retData, length);
	if(result > 0){
		return QString(retData);
	}
	return QString("error");
}
