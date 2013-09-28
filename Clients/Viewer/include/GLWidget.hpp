/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _GLWIDGET_HPP_
#define _GLWIDGET_HPP_

#include <QGLWidget>

class QTimer;

namespace SphereSim{
	/**
	 * \brief Defines the OpenGL widget of the main window.
	 */
	class GLWidget : public QGLWidget{
		Q_OBJECT
	private:
		void initializeGL();
		void resizeGL(int width, int height);
		void paintGL();
		void paintBackground();
		
		QTimer* updateTimer;
	public:
		GLWidget(QWidget* parent);
	};
}

typedef SphereSim::GLWidget GLWidget;

#endif
