/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ActionServer.hpp"
#include "Connection.hpp"
#include "Console.hpp"

#include <QCoreApplication>
#include <string>

Q_DECLARE_METATYPE(std::string);

using namespace SphereSim;

/**
 * method main:
 * Creates instance of ActionServer to listen to incoming client connections.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv)
{
    qRegisterMetaType<std::string>();

    QCoreApplication app(argc, argv);
    try
    {
        ActionServer actSvr(Connection::listeningAddress, Connection::port);
        return app.exec();
    }
    catch (std::exception ex)
    {
        Console()<<Console::red<<Console::bold
            <<"Exception caught: "<<ex.what()<<"\n";
        return 1;
    }
}
