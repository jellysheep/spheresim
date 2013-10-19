/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _MAINWINDOW_HPP_
#define _MAINWINDOW_HPP_

#include <QMainWindow>

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
		
		ActionSender* actionSender;
		
	public:
		/** \brief Initialize member variables. */
		MainWindow(ActionSender* actionSender, QWidget* parent = NULL);
		
		/** \brief Clean up member variables. */
		~MainWindow();
		
	};
}

/** \brief Make MainWindow known to Qt GUI classes. */
typedef SphereSim::MainWindow MainWindow;

#endif
