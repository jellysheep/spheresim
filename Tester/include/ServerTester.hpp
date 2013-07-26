
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
		
		~ServerTester();
		
		/*
		 * method runTests:
		 * Runs all tests (of a specific action group) on the server and verifies the replies.
		 */
		void runTests();
		void runTests(ActionGroups::Group actionGroup);
		
		/*
		 * method runBasicActionTests:
		 * Runs all tests of the basic actions.
		 */
		void runBasicActionTests();
		
		/*
		 * method verify:
		 * Verifies a comparison.
		 */
		#define verify(t1,op,t2)							\
			verify##op(t1,t2,TOSTR(t1),TOSTR(t2));
		#define verifyFunc(name,op,invOp)					\
		template<typename T1, typename T2>					\
		void verify##name(T1 t1, T2 t2, 					\
				const char* nameT1, const char* nameT2){	\
			Console::out<<"ServerTester: ";					\
			Console::bold<<"test "<<++testCounter<<": ";	\
			if(t1 op t2){									\
				Console::greenBold<<"test passed.\n";		\
				successCounter++;							\
			}else{											\
				Console::redBold<<"test failed: ";			\
				Console::out<<"\n              ";			\
				Console::bold<<nameT1;						\
				Console::out<<" [\""<<t1<<"\"] ";			\
				Console::bold<<TOSTR(invOp) " "<<nameT2;	\
				Console::out<<" [\""<<t2<<"\"]\n";			\
			}												\
		}
		verifyFunc(Equal,==,!=);
		verifyFunc(Greater,>,<=);
		verifyFunc(Smaller,<,>=);
		verifyFunc(GreaterOrEqual,>=,<);
		verifyFunc(SmallerOrEqual,<=,>);
	};
	
}

#endif
