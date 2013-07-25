
#include <ServerTester.hpp>
#include <Connection.hpp>

#include <QCoreApplication>

using namespace SphereSim;

/*
 * method main:
 * Creates instance of ServerTester to test server actions.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	ServerTester svrTester(Connection::defaultAddress, Connection::defaultPort);
	svrTester.runTests();
	//app.exec();
	
	return 0;
}
