/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "SimulationGrapher.hpp"
#include "Connection.hpp"
#include "Console.hpp"

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>

using namespace SphereSim;

/**
 * method main:
 * Creates instance of SimulationGrapher to benchmark server simulations.
 * Starts QCoreApplication.
 */

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    try
    {
        SimulationGrapher simGrapher(args, Connection::address, Connection::port);
        return app.exec();
    }
    catch (std::exception ex)
    {
        Console::red<<"Exception caught: "<<ex.what()<<"\n";
        return 1;
    }
}
