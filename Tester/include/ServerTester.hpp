
#ifndef _SERVERTESTER_HPP_
#define _SERVERTESTER_HPP_

#include <Actions.hpp>
#include <Console.hpp>
#include <Version.hpp>

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
		int testCounter;
		int successCounter;
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
		 * Verifies a comparison.
		 */
		#define verify(name,op,invOp)						\
		template<typename T1, typename T2>					\
		void verify##name(T1 t1, T2 t2){					\
			Console::out<<"ServerTester: ";					\
			Console::bold<<"test "<<++testCounter<<": ";	\
			if(t1 op t2){									\
				Console::greenBold<<"test passed.\n";		\
				successCounter++;							\
			}else{											\
				Console::redBold<<"test failed: ";			\
				/*Console::out<<"ServerTester: ";*/			\
				Console::out<<t1;							\
				Console::bold<<" " TOSTR(invOp) " ";		\
				Console::out<<t2<<"\n";						\
			}												\
		}
		verify(Equal,==,!=);
		verify(Greater,>,<=);
		verify(Smaller,<,>=);
		verify(GreaterOrEqual,>=,<);
		verify(SmallerOrEqual,<=,>);
	};
	
}

#endif
