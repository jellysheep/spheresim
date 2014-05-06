/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _MAINWINDOW_HPP_
#define _MAINWINDOW_HPP_

#include "Vector.hpp"

#include <QMainWindow>
#include <QElapsedTimer>

namespace Ui
{
    class MainWindow;
}

namespace SphereSim
{
    class ActionSender;
    class SystemCreator;

    /** \brief Custom main window handling the widgets. */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    private:
        /** \brief Qt UI object containing all widgets. */
        Ui::MainWindow* ui;

        /** \brief ActionSender used to send signals. */
        ActionSender* actionSender;

        QElapsedTimer timer;

        Scalar boxLength;

        unsigned char systemToPrepare;

        SystemCreator* systemCreator;

        void prepareSystem1(unsigned short sphCount);

        void prepareSystem2();

        void prepareSystem3(unsigned short sphCount);

        void prepareSystem4();

        void prepareSystem5();

        void updateBoxLength(Scalar length);

    public:
        /** \brief Initialize member variables. */
        MainWindow(const char* addr, unsigned short port,
            unsigned short sphCount, QWidget* parent = nullptr);

        /** \brief Clean up member variables. */
        ~MainWindow();

        MainWindow() = delete;
        MainWindow(const MainWindow&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;

    public slots:
        void increaseEnergy();

        void decreaseEnergy();

        void updateTargetTemperature();

        void run();

    };
}

/** \brief Make MainWindow known to Qt GUI classes. */
typedef SphereSim::MainWindow MainWindow;

#endif /*_MAINWINDOW_HPP_*/
