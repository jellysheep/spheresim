/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _MAINWINDOW_HPP_
#define _MAINWINDOW_HPP_

#include <Vector.hpp>

#include <QMainWindow>
#include <QElapsedTimer>
#include <QHostAddress>

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

		quint8 systemToPrepare;

		SystemCreator* systemCreator;

		void prepareSystem1();

		void prepareSystem2();

		void prepareSystem3(quint16 sphCount);

		void prepareSystem4();

		void prepareSystem5();

		void updateBoxLength(Scalar length);

	public:
		/** \brief Initialize member variables. */
		MainWindow(QStringList args, QHostAddress addr, quint16 port,
			quint16 sphCount, QWidget* parent = nullptr);

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

#endif
