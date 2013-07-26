
#include <ActionSender.hpp>
#include <ServerTester.hpp>

#include <QTcpServer>
#include <QProcess>

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

ServerTester::~ServerTester(){
	delete sender;
}

void ServerTester::runTests(){
	runTests(ActionGroups::basic);
}

void ServerTester::runTests(ActionGroups::Group actionGroup){
	testCounter = 0;
	successCounter = 0;
	
	switch(actionGroup){
	case ActionGroups::basic:
		runBasicActionTests();
		break;
	default:
		Console::out<<"ServerTester: ";
		Console::bold<<"Unknown action group requested. \n";
		break;
	}
	
	Console::out<<"ServerTester: ";
	if(testCounter == successCounter){
		Console::greenBold<<"all "<<testCounter<<" tests passed.\n";
	}else{
		Console::redBold<<(testCounter-successCounter)<<" out of "<<testCounter<<" tests failed.\n";
	}
}

void ServerTester::runBasicActionTests(){
	verify(sender->isConnected(), Equal, true);
	if(sender->isConnected()){
		Console::out<<"ServerTester: ";
		Console::bold<<"SphereSim Tester v" VERSION_STR<<'\n';
		Console::out<<"ServerTester: ";
		Console::bold<<sender->getVersion()<<'\n';
	}
	verify(sender->getVersion().length(), Greater, 0);
	verify(sender->getTrueString(), Equal, "true");
}
