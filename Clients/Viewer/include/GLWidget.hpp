/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _GLWIDGET_HPP_
#define _GLWIDGET_HPP_

#include <FrameBuffer.hpp>

#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QGLShaderProgram>

class QTimer;
class QElapsedTimer;

namespace SphereSim
{
	/** \brief Custom OpenGL widget of the main window. */
	class GLWidget : public QGLWidget, protected QGLFunctions
	{
		Q_OBJECT
	private:
		/** \brief Initialize the OpenGL scene. */
		void initializeGL();
		
		void initShaders();
		
		/** \brief Resize the OpenGL widget. */
		void resizeGL(int width, int height);
		
		/** \brief Paint the OpenGL scene. */
		void paintGL();
		
		/** \brief Paint the background. */
		void paintBackground();
		
		FrameBuffer<Sphere>* frameBuffer;
		
		QGLShaderProgram program;
		
		GLuint posAttr;
		
		GLuint colAttr;
		
		GLuint matrixUniform;
		
		quint32 frames;
		
		QMatrix4x4 perspectiveMatrix;
		
		void shaderLoadError();
		
		QTimer* animationTimer;
		
		QElapsedTimer* controlTimer;
		
		int sleepTime;
		
		int frameBufferPercentageLevelSum;
		
		int frameBufferPercentageLevelCounter;
		
		bool animating;
		
	public:
		/** \brief Initialize member variables. */
		GLWidget(QWidget* parent);
		
		/** \brief Clean up member variables. */
		~GLWidget();
		
		void setFrameBuffer(FrameBuffer<Sphere>* frameBuffer);
		
	public slots:
		void timerUpdate();
		
		void updateTimerFrequency(int frameBufferPercentageLevel);
		
		void startAnimation();
		
		void stopAnimation();
	};
}

/** \brief Make GLWidget known to Qt GUI classes. */
typedef SphereSim::GLWidget GLWidget;

#endif
