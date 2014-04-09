/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "StartDialog.hpp"
#include "ActionSender.hpp"
#include "Connection.hpp"
#include "MainWindow.hpp"
#include "Console.hpp"

#include <QApplication>
#include <QDialog>

using namespace SphereSim;

/**
 * method main:
 * Builds up a connection to server and opens main window.
 */

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyle("fusion");

    unsigned short sphereCount = StartDialog().getSphereCount();

    if (sphereCount>0)
    {
        try
        {
            MainWindow mainWindow(Connection::address,
                Connection::port, sphereCount);
            mainWindow.show();

            return app.exec();
        }
        catch (std::exception ex)
        {
            Console()<<Console::red<<Console::bold
                <<"Exception caught: "<<ex.what()<<'\n';
            return 1;
        }
    }
    return 0;
}
