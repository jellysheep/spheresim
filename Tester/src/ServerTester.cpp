
#include <ActionSender.hpp>
#include <ServerTester.hpp>

#include <QTcpServer>

using namespace SphereSim;

ServerTester::ServerTester(const QHostAddress& addr, const quint16 port){
	qDebug()<<"ServerTester: constructor called";
	sender = new ActionSender(addr, port);
}
ServerTester::ServerTester(const QString& addr, const quint16 port)
	:ServerTester(QHostAddress(addr),port){
}
ServerTester::ServerTester(const char* addr, const quint16 port)
	:ServerTester(QString(addr),port){
}

void ServerTester::runTests(){
	verify(sender->getTrueString() == "true");
}

void ServerTester::verify(bool b){
	if(b){
		qDebug()<<"test passed";
	}else{
		qDebug()<<"test failed";
	}
}
