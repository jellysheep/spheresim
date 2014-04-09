/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SERVERTESTER_HPP_
#define _SERVERTESTER_HPP_

#include "Actions.hpp"
#include "Console.hpp"
#include "Version.hpp"

#include <QtGlobal>
#include <QObject>

class ActionSender;

namespace SphereSim
{
    class SystemCreator;

    /** \brief Verifier of server actions. */
    class ServerTester : public QObject
    {
        Q_OBJECT

    private:
        /** \brief Client object to communicate with server. */
        ActionSender* sender;

        /** \brief Number of already done tests. */
        unsigned short testCounter;

        /** \brief Number of succeeded tests. */
        unsigned short successCounter;

        /** \brief Success flag of the current test. */
        bool testSuccess;

        /** \brief Name of the currently tested action. */
        std::string testActionName;

        /** \brief Dummy variable to name the framebuffer test. */
        static const int framebuffer;

        /** \brief Test result (0: all tests passed, 1 = at least one failed). */
        unsigned short testResult;

        SystemCreator* systemCreator;

    public:
        /** \brief Start a ServerTester with the specified address and port.
         * \param addr The address that the socket will be connecting to.
         * \param port The port that the socket will be connecting to. */
        ServerTester(const char* addr, unsigned short port);

        ~ServerTester();

        ServerTester() = delete;
        ServerTester(const ServerTester&) = delete;
        ServerTester& operator=(const ServerTester&) = delete;

        /** \brief Run all tests of a specific action group on the server and
         * verify the replies.
         * \param actionGroup Group of the actions that will be tested.
         * \param groupName Action group name. Used for console outputs. */
        void runTests(unsigned char actionGroup, const char* groupName);

        /** \brief Verification of all BasicActions. */
        void runBasicActionTests();

        /** \brief Verification of all SpheresUpdatingActions. */
        void runSpheresUpdatingActionTests();

        /** \brief Verification of all CalculationActions. */
        void runCalculationActionTests();

        /** \brief Simulation of a bouncing sphere.
         * \param integratorMethod Name of the used integrator method. */
        void runCalculationActionTests_internal(unsigned char order,
            const char* integratorMethod);

        /** \brief Verification of the FrameBuffer. */
        void runFrameBufferTests();

        /** \brief Verification of a comparison and result printing on console. */
        #define verify(t1, op, t2)                          \
            verify##op(t1, t2, __LINE__, TOSTR(t1), TOSTR(t2));
        /** \brief Declaration of different verification methods. */
        #define verifyFunc(name, op, invOp)                 \
        template <typename T1, typename T2>                 \
        void verify##name(T1 t1, T2 t2, unsigned short line,\
                const char* nameT1, const char* nameT2)     \
        {                                                   \
            if (testSuccess == false)                       \
            {                                               \
                return;                                     \
            }                                               \
            if ((t1 op t2) == false)                        \
            {                                               \
                Console()<<Console::red<<Console::bold<<"test failed: ";             \
                Console()<<"\n              ";              \
                Console()<<Console::white<<Console::bold<<nameT1;                    \
                Console()<<" [\""<<t1<<"\"] ";              \
                Console()<<Console::white<<Console::bold<<TOSTR(invOp) " "<<nameT2;  \
                Console console;                            \
                console<<" [\""<<t2<<"\"]";                 \
                console<<"\n              ";                \
                console<<"(line "<<line;                    \
                if (testActionName.size()>0)                \
                {                                           \
                    console<<", "<<testActionName;          \
                }                                           \
                console<<")\n";                             \
                console.flush();                            \
                testSuccess = false;                        \
                testResult = 1;                             \
            }                                               \
        }
        verifyFunc(Equal, ==, !=);
        verifyFunc(Greater, >, <=);
        verifyFunc(Smaller, <, >=);
        verifyFunc(GreaterOrEqual, >=, <);
        verifyFunc(SmallerOrEqual, <=, >);

        /** \brief Declaration of an approximately-equal verification method. */
        #define verifyApproxEqual(t1, t2, line, str1, str2) \
            verifySmallerOrEqual(fabs(t1-t2),               \
            0.0001*(fabs(t1)+fabs(t2)),                     \
            line, "|" str1 " - " str2 "|",                  \
            "0.0001*(|" str1 "| + |" str2 "|)");

        /** \brief Introduction of a new test.
         * \param actionName Name of the action that will be tested. */
        void startTest(const char* actionName);

        /** \brief Completion of a test. */
        void endTest();

        /** \brief Completion of a test and introduction of a new test.
         * \copydetails startTest */
        void startNewTest(const char* actionName);

        /** \copydoc testResult
         * \return Test result value. */
        unsigned short result();

    public slots:
        /** \brief Verification of all actions. */
        void run();

    };

}

#endif
