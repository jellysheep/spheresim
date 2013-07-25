
#ifndef _SERVERTESTER_HPP_
#define _SERVERTESTER_HPP_

#include <Actions.hpp>

#include <QtGlobal>

class ActionSender;
class QHostAddress;

namespace SphereSim{
	
	/*
	 * class ServerTester:
	 * Sends actions to server and verifies server replies.
	 */
	class ServerTester{
	private:
		ActionSender* sender;
	public:
		/*
		 * constructor:
		 * Starts a ServerTester with the specified address and port.
		 */
		ServerTester(const QHostAddress& addr, const quint16 port);
		ServerTester(const QString& addr, const quint16 port);
		ServerTester(const char* addr, const quint16 port);
		
		/*
		 * method runTests:
		 * Runs all tests on the server and verifies replies.
		 */
		void runTests();
		
		/*
		 * method verify:
		 * Verifies a boolean result.
		 */
		void verify(bool b);
	};
	
}

#endif
