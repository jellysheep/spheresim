/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SERVERTESTER_HPP_
#define _SERVERTESTER_HPP_

#include <Actions.hpp>
#include <Console.hpp>
#include <Version.hpp>

#include <QtGlobal>

class ActionSender;
class QHostAddress;

namespace SphereSim{
	
	/**
	 * \brief Sends actions to server and verifies server replies.
	 */
	class ServerTester{
	private:
		/** \brief Holds the client object to communicate with server. */
		ActionSender* sender;
		
		/** \brief Tells how many tests have been done. */
		unsigned int testCounter;
		
		/** \brief Tells how many of the tests have been successful. */
		unsigned int successCounter;
		
		/** \brief Tells if the current test is successful. */
		bool testSuccess;
		
		/** \brief Tells which action is currently tested. */
		QString testActionName;
		
	public:
		/**
		 * \brief Starts a ServerTester with the specified address and port.
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerTester(const QHostAddress& addr, const quint16 port);
		/**
		 * \brief Starts a ServerTester with the specified address and port. 
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerTester(const QString& addr, const quint16 port);
		/**
		 * \brief Starts a ServerTester with the specified address and port. 
		 * \param addr The address that the socket will be connecting to.
		 * \param port The port that the socket will be connecting to.
		 */
		ServerTester(const char* addr, const quint16 port);
		
		~ServerTester();
		
		/**
		 * \brief Runs all tests on the server and verifies the replies.
		 */
		void runTests();
		/**
		 * \brief Runs all tests of a specific action group on the server and verifies the replies.
		 * \param actionGroup Group of the actions that will be tested.
		 * \param groupName Name of the specified action group. Used for console outputs.
		 */
		void runTests(ActionGroups::Group actionGroup, const char* groupName);
		
		/** \brief Runs all tests of the basic actions. */
		void runBasicActionTests();
		
		/** \brief Runs all tests of the sphere updating actions. */
		void runSpheresUpdatingActionTests();
		
		/** \brief Verifies a comparison and displays result on console. */
		#define verify(t1,op,t2)							\
			verify##op(t1,t2,__LINE__,TOSTR(t1),TOSTR(t2));
		#define verifyFunc(name,op,invOp)					\
		template<typename T1, typename T2>					\
		void verify##name(T1 t1, T2 t2, unsigned int line,	\
				const char* nameT1, const char* nameT2){	\
			if(!testSuccess) return;						\
			if(t1 op t2){									\
			}else{											\
				Console::redBold<<"test failed: ";			\
				Console::out<<"\n              ";			\
				Console::bold<<nameT1;						\
				Console::out<<" [\""<<t1<<"\"] ";			\
				Console::bold<<TOSTR(invOp) " "<<nameT2;	\
				Console::out<<" [\""<<t2<<"\"]";			\
				Console::out<<"\n              ";			\
				Console::out<<"(line "<<line;				\
				if(testActionName.length()>0){				\
					Console::out<<", "<<testActionName;		\
				}											\
				Console::out<<")\n";						\
				testSuccess = false;						\
			}												\
		}
		verifyFunc(Equal,==,!=);
		verifyFunc(Greater,>,<=);
		verifyFunc(Smaller,<,>=);
		verifyFunc(GreaterOrEqual,>=,<);
		verifyFunc(SmallerOrEqual,<=,>);
		
		/**
		 * \brief Informs that a new test will be started.
		 * \param actionName Name of the action that will be tested.
		 */
		void startTest(const char* actionName);
		
		/** \brief Informs that a test is complete. */
		void endTest();
		
		/**
		 * \brief Informs that a test is complete and a new test will be started.
		 * \param actionName Name of the action that will be tested.
		 */
		void startNewTest(const char* actionName);
	};
	
}

#endif
