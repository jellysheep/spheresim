
#include <ActionSender.hpp>
#include <Connection.hpp>

#include <QCoreApplication>
#include <QString>
#include <QDebug>

using namespace SphereSim;

/*
 * method main:
 * Creates instance of ActionSender to build up connection to server.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	ActionSender actSend(Connection::defaultAddress, Connection::defaultPort);
	const QString version = actSend.getVersion();
	qDebug()<<version;
	const QString trueStr = actSend.getTrueString();
	qDebug()<<trueStr;
	//app.exec();
	
	return 0;
}
