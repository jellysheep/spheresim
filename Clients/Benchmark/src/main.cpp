/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ServerBenchmark.hpp"
#include "Connection.hpp"
#include "Console.hpp"

#include <QCoreApplication>
#include <QTimer>

using namespace SphereSim;

/**
 * method main:
 * Creates instance of ServerBenchmark to benchmark server simulations.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    try
    {
        ServerBenchmark svrBenchmark(Connection::address, Connection::serverRecvPort,
            Connection::serverSendPort);
        return app.exec();
    }
    catch (std::exception ex)
    {
        Console()<<Console::red<<Console::bold
            <<"Exception caught: "<<ex.what()<<'\n';
        return 1;
    }
}
