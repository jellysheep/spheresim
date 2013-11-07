/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _MAINWINDOW_HPP_
#define _MAINWINDOW_HPP_

#include <Sphere.hpp>

#include <QMainWindow>
#include <QElapsedTimer>

namespace Ui
{
	class MainWindow;
}

namespace SphereSim
{
	class ActionSender;
	
	/** \brief Custom main window handling the widgets. */
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	private:
		/** \brief Qt UI object containing all widgets. */
		Ui::MainWindow* ui;
		
		/** \brief ActionSender used to send signals. */
		ActionSender* actionSender;
		
		void prepareSystem1();
		
		void prepareSystem2();
		
		void prepareSystem3();
		
		QElapsedTimer timer;
		
		Scalar boxLength;
		
		void updateBoxLength(Scalar length);
		
		quint8 systemToPrepare;
		
	public:
		/** \brief Initialize member variables. */
		MainWindow(ActionSender* actionSender, QWidget* parent = NULL);
		
		/** \brief Clean up member variables. */
		~MainWindow();
		
	public slots:
		void increaseEnergy();
		
		void decreaseEnergy();
		
		void updateTargetTemperature();
		
	};
}

/** \brief Make MainWindow known to Qt GUI classes. */
typedef SphereSim::MainWindow MainWindow;

#endif
